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
string AUDIO_FILE = "shitters.flac";



int main(int argc, char* argv[])
{
    system("clear");

    GOOGLE_PROTOBUF_VERIFY_VERSION;


    SyncRecognizeRequest sync_recognize_request;
    RecognitionConfig config;

    // SETUP CONFIG //
    ::google::protobuf::int32 a = 16000;
    ::google::protobuf::int32 b = 1;
    google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding encoding = google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_FLAC;

    config.set_encoding(encoding);
    config.set_sample_rate(a);
    config.set_max_alternatives(b);
    config.set_profanity_filter(false);

    sync_recognize_request.set_allocated_config(&config);


    // RECORD AUDIO AND SAVE TO FILE //
    unsigned int sampleRate = 16000; //44100

    if (!sf::SoundBufferRecorder::isAvailable())
    {
        cerr << "Error: Audio Device not aviable" << endl;
        return -1;
    }

    sf::SoundBufferRecorder recorder;
    if (!recorder.start(sampleRate))
    {
        cerr << "Error starting recorder" << endl;
        return -1;
    }

    while (cin.get() != '\n'){}
    recorder.stop();

    const sf::SoundBuffer& buffer = recorder.getBuffer();
    buffer.saveToFile(AUDIO_FILE);

    // LOAD IN THAT AUDIO FILE //
    ifstream infile;
    infile.open(AUDIO_FILE);
    string buf { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    infile.close();


    //--------------------------//
    /// LOAD RECOGNITION AUDIO ///
    //--------------------------//

    RecognitionAudio audio;
    audio.set_content(buf);
    sync_recognize_request.set_allocated_audio(&audio);


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
      // cout << s.error_code() << ": " << s.error_message() << s.ok() << endl;
      cerr << "RPC failed" << endl;;
    }


    cout << "\nAll Finished!" << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

//////////////////
// SET UP OAUTH //
//////////////////
