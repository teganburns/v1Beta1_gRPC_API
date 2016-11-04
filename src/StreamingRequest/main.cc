#include <bitset>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <pthread.h>
#include <typeinfo>
#include <thread>


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

using namespace std;

#include <grpc++/grpc++.h>
#include "cloud_speech.grpc.pb.h"


using google::cloud::speech::v1beta1::SyncRecognizeRequest;
using google::cloud::speech::v1beta1::AsyncRecognizeRequest;
using google::cloud::speech::v1beta1::AsyncRecognizeResponse;
using google::cloud::speech::v1beta1::Operation;
using google::cloud::speech::v1beta1::RecognitionAudio;
using google::cloud::speech::v1beta1::RecognitionConfig;
using google::cloud::speech::v1beta1::SpeechContext;
using google::cloud::speech::v1beta1::SpeechRecognitionAlternative;
using google::cloud::speech::v1beta1::SpeechRecognitionResult;
using google::cloud::speech::v1beta1::StreamingRecognitionConfig;
using google::cloud::speech::v1beta1::StreamingRecognitionResult;
using google::cloud::speech::v1beta1::StreamingRecognizeRequest;
using google::cloud::speech::v1beta1::StreamingRecognizeResponse;
using google::cloud::speech::v1beta1::SyncRecognizeRequest;
using google::cloud::speech::v1beta1::SyncRecognizeResponse;

using grpc::CompletionQueue;
using grpc::Channel;
using grpc::RpcService;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;

using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using google::cloud::speech::v1beta1::Speech;

string line = "-----------------------------------------------------------------";
string SCOPE = "speech.googleapis.com";
bool finished;


class MyRecorder : public sf::SoundRecorder
{
    virtual bool onStart()
    {
        cout << "Recorder Started" << endl;
        loop_count = 0;
        ready = false;

        return true;
    }

    virtual bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount )
    {
        sample_count  = sampleCount;
        samples_ = samples;
        loop_count++;

        ready = true;
        while (ready){} // poor implmentation of a loop while we wait for samples to be processed //

        return true;
    }

    virtual void onStop()
    {
        ready = false;
        cout << "Recorder Stoped" << endl;
    }


public:

    int sample_rate = 16000;
    std::size_t sample_count;
    int loop_count;
    const int16_t* samples_;
    bool ready;

}; // end class MyRecorder



void ReadMessages(void *stream_ptr)
{

    std::shared_ptr<ClientReaderWriter<StreamingRecognizeRequest, StreamingRecognizeResponse> > *stream_holder;
    stream_holder = (std::shared_ptr<ClientReaderWriter<StreamingRecognizeRequest, StreamingRecognizeResponse> > *) stream_ptr;
    std::shared_ptr<ClientReaderWriter<StreamingRecognizeRequest, StreamingRecognizeResponse> > stream = *stream_holder;

    // print information for debugging purposes
/*
    cout << "\n[THREAD]Stream type: " << typeid(stream).name() << endl;
    cout << "[THREAD]Stream ref type: " << typeid(&stream).name() << endl;
    cout << "[THREAD]stream: " << stream << endl;
    cout << "[THREAD]Stream ref: " << &stream << endl;
    cout << "[THREAD]stream.get(): " << stream.get() << endl;
    cout << "[THREAD]stream.get() type: " << typeid(stream.get()).name() << endl;
*/

    StreamingRecognizeResponse response;
    Status status_;
    int while_loop = 0;
    int previous_result_size = 0;


    while (stream->Read(&response)){

        cout << "[THREAD] while_loop: " << while_loop++ << endl;
        if (status_.ok()){
            cout << "\n[THREAD]status_.ok!" << endl;
            // Check Endpointer Type //

            if (response.endpointer_type() == ::google::cloud::speech::v1beta1::StreamingRecognizeResponse_EndpointerType_ENDPOINTER_EVENT_UNSPECIFIED){cout <<  "[THREAD]Endpointer unspecified" << endl;}
            else if (response.endpointer_type() == ::google::cloud::speech::v1beta1::StreamingRecognizeResponse_EndpointerType_START_OF_SPEECH){cout <<  "[THREAD]Endpointer START_OF_SPEECH" << endl;}
            else if (response.endpointer_type() == ::google::cloud::speech::v1beta1::StreamingRecognizeResponse_EndpointerType_END_OF_SPEECH){cout <<  "[THREAD]Endpointer END_OF_SPEECH" << endl;}
            else if (response.endpointer_type() == ::google::cloud::speech::v1beta1::StreamingRecognizeResponse_EndpointerType_END_OF_AUDIO){cout <<  "[THREAD]Endpointer END_OF_AUDIO" << endl;}
            else if (response.endpointer_type() == ::google::cloud::speech::v1beta1::StreamingRecognizeResponse_EndpointerType_END_OF_UTTERANCE){cout << "[THREAD]Endpointer END_OF_UTTERANCE" << endl;}
            else{cout << "[THREAD]ENDPOINTER TYPE UNKNOWN CODE MAY BREAK!!" << endl;}
            //cout << "[THREAD]Type id: " << typeid(response.result_index()).name() << endl;

            cout << "[THREAD]Reponse.result_index(): " << response.result_index() << endl;
            cout << "[THREAD]Response.results_size(): " << response.results_size() << endl;

            if (response.results_size() > previous_result_size )
            {

                response.set_result_index(previous_result_size); cout << "[THREAD]response index set" << endl;
                StreamingRecognitionResult result_ = response.results(previous_result_size);
                cout << "RESULT OK" << endl;
                cout << "[THREAD]Result_.alternatives_size(): " << result_.alternatives_size() << endl;
                cout << "[THREAD]Result_.stability(): " << result_.stability() << endl;


                for (int i = 0; i < result_.alternatives_size(); i++)
                {
                    SpeechRecognitionAlternative alternative_ = result_.alternatives(i);
                    cout << "\n[THREAD]alternative: " << i << endl;
                    cout << "[THREAD]alternative_.confidence: " << alternative_.confidence() << endl;
                    cout << "[THREAD]alternative_.transcript(): " << alternative_.transcript() << endl;
                    //if (alternative_.transcript() == "exit" || alternative_.transcript() == "quit")
                    //{ cout << "[THREAD]exit called breaking!!"; finished = true;}
                }

                if (result_.is_final()){ cout << "[THREAD]Result_.is_final(): TRUE" << endl; }
                if (!result_.is_final()){ cout << "[THREAD]Result_.is_final(): FALSE" << endl;}


            }




        }else{
            cout << "\n[THREAD]Status NOT OK!! " << endl;
            cout << "[THREAD]Status Code: " << status_.error_code() << "\n[THREAD]Status Message: " << status_.error_message() << endl; break;}

    }
    cout << "[THREAD]Thread while loop exited" << endl;

}


// this is called when creating a new thread in main
// it's a crude way to implment a timeout without holding up the main thread
void timeout_method(void *timeout_ptr){


    bool *timeout_holder;
    timeout_holder = (bool *) timeout_ptr;
    bool timeout = *timeout_holder;

    cout << "[Thread]Timeout address: " << timeout_holder << "        Timeout state: " << *timeout_holder <<  endl;
    sleep(20);
    cout << "TIMEOUT!!" << endl;

    *timeout_holder = true;
    }



int main(int argc, char* argv[])
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    finished = false;


    //------------------//
    //  START  RECORDER //
    //------------------//
    // we do this early to give it time to initalize //

    if (!MyRecorder::isAvailable()) {cerr << "Error: Audio Device not aviable" << endl;return -1;}
    MyRecorder recorder;

    // this will print out the aviable audio devices refer to SFML's website for more information //

/*
    vector<string> default_devices_ = recorder.getAvailableDevices();
    for (string e : default_devices_){
        cout << "AviableDevices: " << e << endl;
    }
    cout  << "DefaultDevice: " << recorder.getDefaultDevice() << endl;
*/

    // this is the card that my computer had, you will more than likely need to change this //
    const string Intel_Device = "HDA Intel PCH, 92HD91BXX Analog (CARD=PCH,DEV=0)";
    recorder.setDevice(Intel_Device);
    if (!recorder.start(recorder.sample_rate)) {cerr << "Error starting recorder" << endl; return -1;}

    cout << "SampleRate: " << recorder.getSampleRate() << "        Device: " << recorder.getDevice() << "        ChannelCount: " << recorder.getChannelCount() << endl;


    //-------------//
    // SET UP SRR  //
    //-------------//
    StreamingRecognizeRequest streaming_recognize_request;
    StreamingRecognitionConfig streaming_config;
    RecognitionConfig config;
    streaming_recognize_request.set_allocated_streaming_config(&streaming_config);
    streaming_config.set_allocated_config(&config);


    // CONFIGURE CONFIG //
    ::google::protobuf::int32 sample_rate = recorder.sample_rate;
    ::google::protobuf::int32 max_alternatives = 5;
    google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding encoding = google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_LINEAR16;

    config.set_encoding(encoding);
    config.set_sample_rate(sample_rate);
    config.set_max_alternatives(max_alternatives);
    config.set_profanity_filter(false);

    // DISPLAY RESULTS FOR DEBUGGING //
    // cout << "\n--Quick config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;


    // CONFIGURE STREAMING_CONFIG //
    bool single_utterance = false;
    bool interim_results = true;

    streaming_config.set_single_utterance(single_utterance);
    streaming_config.set_interim_results(interim_results);



    //-------------------------------//
    //  OPEN CHANNEL AND SEND CONFIG //
    //-------------------------------//

    cout << "Attempting to send config to server" << endl;

    grpc::ClientContext context;
    StreamingRecognizeResponse response;


    auto creds = grpc::GoogleDefaultCredentials();
    auto channel = grpc::CreateChannel(SCOPE, creds);
    std::unique_ptr<Speech::Stub> stub_(Speech::NewStub(channel));
    std::shared_ptr<ClientReaderWriter<StreamingRecognizeRequest, StreamingRecognizeResponse> > stream(stub_->StreamingRecognize(&context));

    cout << "Finished opening channel" << endl;

    // DISPLAY STREAM INFO  FOR DEBUGGING //
/*
    cout << "\n\nStream type: " << typeid(stream).name() << endl;
    cout << "Stream ref type: " << typeid(&stream).name() << endl;
    cout << "stream: " << stream << endl;
    cout << "Stream ref: " << &stream << endl;
    cout << "stream.get(): " << stream.get() << endl;
    cout << "stream.get() type: " << typeid(stream.get()).name() << endl;

    cout << "\n\nStub type: " << typeid(stub_).name() << endl;
    cout << "Stub ref type: " << typeid(&stub_).name() << endl;
    cout << "Stub ref: " << &stub_ << endl;
*/

    cout << "Sending config file" << endl;
    stream->Write(streaming_recognize_request);

    streaming_config.release_config();
    streaming_recognize_request.release_streaming_config();

    // CREATE NEW THREAD FOR READING INCOMMING MESSAGES //
    cout << "Reading Messages in new thread" << endl;
    std::thread thread_(ReadMessages, &stream);


    // SOME VARS FOR WHILE LOOP //
    int sc;
    string audio_content;
    int cnt = 0;
    bool timeout = false;
    std::thread timeout_thread_(timeout_method, &timeout);
    //cout << "Timeout address: " << &timeout << "    Timeout state: " << timeout <<  endl;

    streaming_recognize_request.set_allocated_audio_content(&audio_content);


    cout << "Entering While loop" << endl;

    // SAVE AUDIO STREAM FOR REFRENCE INCASE THERE ARE ERRORS //
    //ofstream outfile;
    //outfile.open("file.dat", ios::out);


    while (!finished && !timeout)
    {

        while (!recorder.ready && !timeout) {  }
        if (timeout){break;}
        //cout << "cnt: " << cnt++ << endl;

        // PUT ALL THE AVIABLE SAMPLES IN A BUFFER
        for (unsigned long i=0; i<=recorder.sample_count; i++)
        {
            audio_content += recorder.samples_[i];
        }

        //cout << sizeof(audio_content) << endl;
        //outfile << audio_content;


        //------------------------------------//
        //  SEND STREAMING RECOGNIZE REQUEST  //
        //------------------------------------//
        if (!stream->Write(streaming_recognize_request)) {cout << "broken stream" << endl;break; }
        else{ recorder.ready = false; audio_content = "";};

    }

    //----------------------------------------------------------------------//
    // FINISH STREAM, JOIN THREAD, STOP RECORDER, AND GET FINAL STATUS CODE //
    //----------------------------------------------------------------------//

    recorder.ready = false; // SET TO FALSE SO THE RECORDER DOESNT HANG //
    cout << "Out of while loop!" << endl;

    //outfile.close();

    cout << "stream->WritesDone" << endl;
    if (!stream->WritesDone()) {cout << "Writes unsucessful" << endl;}

    cout << "Joining Thread"  << endl;
    thread_.join();

    cout << "Stopping Recorder" << endl;
    if (recorder.ready){ recorder.ready = false; recorder.stop(); }

    cout << "Getting status" << endl;
    Status status = stream->Finish();
    if(status.ok()) {cout << "Status OK!" <<  endl;}
    if(!status.ok()){cout << "\nStatus error code: " << status.error_code() << "\nStatus error message: " << status.error_message() <<  endl;}


    cout << "\nAll Finished!" << endl;
    ::google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
