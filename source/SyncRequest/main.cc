#include <iostream>
#include <unistd.h>
#include <numeric>
#include <vector>
#include <bitset>
#include <fstream>
#include <typeinfo>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#include <sox.h>

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
//using google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding;

// using grpc::Channel;
// using grpc::ClientContext;
// using grpc::Status;

using grpc::CompletionQueue;
using grpc::Channel;
using grpc::RpcService;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using google::cloud::speech::v1beta1::Speech;

string line = "-----------------------------------------------------------------";
string SCOPE = "speech.googleapis.com";

RecognitionConfig Quick_Config(RecognitionConfig config){


    ::google::protobuf::int32 a = 16000;
    ::google::protobuf::int32 b = 1;
    google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding encoding = google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_FLAC;

    config.set_encoding(encoding);
    config.set_sample_rate(a);
    config.set_max_alternatives(b);
    config.set_profanity_filter(false);

 // cout << "\n--Quick config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;

    return config;

}



void Make_New_Config(char * CONFIG_FILE){

    SyncRecognizeRequest sync_recognize_request;
    RecognitionConfig config;

    // cout << "SRR has_config() [should be false]: " << sync_recognize_request.has_config() << endl;
    sync_recognize_request.set_allocated_config(&config);
    // cout << "set_allocated_config done..." << "\nConfig Adress: " << &config << endl;
    config = Quick_Config(config);
    // cout << "Quick_Config done...\n" << endl;
    // cout << "SRR has_config() [should be true]: " << sync_recognize_request.has_config() << endl;

    // WRITE CONFIG TO CONFIG FILE //
    fstream output(CONFIG_FILE, ios::out | ios::trunc | ios::binary);
    if (!sync_recognize_request.SerializeToOstream(&output)) {
        cerr << "Failed to write CONFIG_FILE." << endl;
    }else{
        cout << "Your CONFIG_FILE has been made. Please run this program again." << endl;
    }

    output.close();

}


int main(int argc, char* argv[])
{
    system("clear");

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // VERIFY  ARGS //
     if (argc < 2) {
    	cerr << "Usage:  " << argv[0] << " AUDIO_FILE[Required]  CONFIG_FILE[Optional]" << endl;
    	return -1;
    }


    SyncRecognizeRequest sync_recognize_request;
    RecognitionConfig config;

    // ATTEMPT TO LOAD CONFIG FILE //
    bool make_new_config = false;
    fstream input(argv[2], ios::in | ios::binary);
    if (!input) {
        cout << argv[2] << ": File not found.  Creating a new file." << endl;
        make_new_config = true;
    } else if (!sync_recognize_request.ParseFromIstream(&input)) {
        cerr << "Failed to parse config file" << endl;
        return -1;
    } else {config = sync_recognize_request.config(); cout << "Config File Loaded" << endl;}
    input.close();
    // sleep(2);
    system("clear");

    // MAKE NEW CONFIG_FILE IF NECESSARY //
    if (make_new_config){
        Make_New_Config(argv[2]);
        return 0;
    }

    // DISPLAY CONFIG_FILE RESULTS //
    //cout << "\n--Config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSetProfanity Filter: " << config.profanity_filter() << endl;
    //sleep(5);
    //system("clear");


    // ATTEMPT TO LOAD AUDIO FILE //
    sf::SoundBuffer audio_buffer;
    if (!audio_buffer.loadFromFile(argv[1]))
    {
        cerr << "Coundn't load from audio file (FLAC)" << endl;
        return -1;

    }else {

    }

    ifstream infile;
    infile.open(argv[1]);
    string buf { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    //cout << buf;
    infile.close();

    // cout << "--Audio File Loaded--" << endl; 
    // cout << "Audio File: " << argv[1] << endl;

    // ofstream file("binary", ios::out | ios::in);
    // file << buf << endl;
    // file.close();


    // PARSE SAMPLES FROM AUDIO BUFFER //

    //const sf::Int16* samples = audio_buffer.getSamples();
    // sf::Int16 pos = samples;
    // std::string binary;
    // for (unsigned int i = 0; i < audio_buffer.getSampleCount(); i++)
    // {
    //     binary += to_string(pos); //std::bitset<16>(pos).to_string();
    //     pos = samples++;
    // }

    // WRITE PARSED SAMPLES TO FILE //
    // fstream file("binary", ios::out | ios::in);
    // file << samps << endl;
    // file.close();

    // DISPLAY AUDIO INFORMATION //
    // cout << "Duration: " << audio_buffer.getDuration().asSeconds() << endl;
    // cout << "Channel Count: " << audio_buffer.getChannelCount() << endl;
    // cout << "Sample Rate: " << audio_buffer.getSampleRate() << endl;
    // cout << "Sample Count: " << audio_buffer.getSampleCount() << endl;

    // sleep(5);
    // system("clear");

    //--------------------------//
    /// LOAD RECOGNITION AUDIO ///
    //--------------------------//

    RecognitionAudio audio;


    audio.set_content(buf);
    cout << "sync_recognize_request.has_audio()";
    if(sync_recognize_request.has_audio()){cout << " returned true" << endl;}else{cout << "returned false" << endl;}

    sync_recognize_request.set_allocated_audio(&audio);

    cout << "sync_recognize_request.has_audio()";
    if(sync_recognize_request.has_audio()){cout << " returned true" << endl;}else{cout << "returned false" << endl;}

    // sleep(5);
    system("clear");

    // std::string n;
    // sync_recognize_request.SerializeToString(&n);

    // WRITE SerializeToString OUTPUT TO FILE //
    // fstream ser_file("SerializeToString", ios::out | ios::in);
    // ser_file << n << endl;
    // ser_file.close();


    //---------------------------------//
    /// OPEN CHANNEL AND SEND REQUEST ///
    //--------------------------------//

    cout << "Attempting to send request to server" << endl;

    // ALLOCATE MEMORY FOR RESPONSES //

    grpc::ClientContext context;
    SyncRecognizeResponse response;

    // Open Channel with Google's Server //
    auto creds = grpc::GoogleDefaultCredentials();
    // Create a channel, stub and make RPC calls (same as in the previous example)
    auto channel = grpc::CreateChannel(SCOPE, creds);
    std::unique_ptr<Speech::Stub> stub(Speech::NewStub(channel));
    grpc::Status s = stub->SyncRecognize(&context, sync_recognize_request, &response);

    cout << "s.ok(): " << s.ok() << endl;

    if (s.ok()) {
        cout << "Status returned OK\nResponse Size :" << response.results_size() << endl;
        int a = 0;
        SpeechRecognitionResult results = response.results(0);
        SpeechRecognitionAlternative alternative = results.alternatives(a);
        cout << "Response :" << alternative.transcript() << endl;
    } else if (s.ok()){
        cout << "Status Returned Canceled" << endl;
    }else {
      cout << s.error_code() << ": " << s.error_message() << s.ok() << endl;
      cerr << "RPC failed" << endl;;
    }

    // Prompt_Audio(sync_recognize_request);

    cout << "\nAll Finished!" << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

//////////////////
// SET UP OAUTH //
//////////////////
