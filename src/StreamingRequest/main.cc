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
    ::google::protobuf::int32 b = 5;
    google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding encoding = google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_FLAC;

    config.set_encoding(encoding);
    config.set_sample_rate(a);
    config.set_max_alternatives(b);
    config.set_profanity_filter(false);

    cout << "\n--Quick config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;

    return config;

}



void Make_New_Config(char * CONFIG_FILE){

    StreamingRecognitionConfig streaming_config;
    RecognitionConfig config;



    streaming_config.set_allocated_config(&config);
    config = Quick_Config(config);

    streaming_config.set_single_utterance(true);
    streaming_config.set_interim_results(true);

    // WRITE CONFIG TO CONFIG FILE //
    fstream output(CONFIG_FILE, ios::out | ios::trunc | ios::binary);
    if (!streaming_config.SerializeToOstream(&output)) {
        cerr << "Failed to write CONFIG_FILE." << endl;
    }else{
        cout << "Your CONFIG_FILE has been made. Please run this program again." << endl;
    }

    output.close();

}

int requestStream()
{


}


int main(int argc, char* argv[])
{
    system("clear");

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // VERIFY  ARGS //
     if (argc < 2) {
    	cerr << "Usage:  " << argv[0] << "  CONFIG_FILE" << endl;
    	return -1;
    }


    StreamingRecognizeRequest streaming_recognize_request;
    StreamingRecognitionConfig streaming_config;
    RecognitionConfig config;

    // ATTEMPT TO LOAD CONFIG FILE //
    bool make_new_config = false;
    fstream input(argv[1], ios::in | ios::binary);
    if (!input) {
        cout << argv[1] << ": File not found.  Creating a new file." << endl;
        make_new_config = true;
    } else if (!streaming_config.ParseFromIstream(&input)) {
        cerr << "Failed to parse config file" << endl;
        return -1;
    } else {config = streaming_config.config(); cout << "Config File Loaded" << endl;}
    input.close();
    sleep(2);
    system("clear");

    // MAKE NEW CONFIG_FILE IF NECESSARY //
    if (make_new_config){
        Make_New_Config(argv[1]);
        return 0;
    }

    // DISPLAY CONFIG_FILE RESULTS //
    cout << "\n--Config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;
    sleep(5);
    system("clear");


    //---------------------------------//
    /// OPEN CHANNEL AND SEND REQUEST ///
    //--------------------------------//

    cout << "Attempting to send request to server" << endl;

    // ALLOCATE MEMORY FOR RESPONSES //
    int DEADLINE_SECS = 8 * 60 * 60;

    grpc::ClientContext context;

    // Open Channel with Google's Server //
    auto creds = grpc::GoogleDefaultCredentials();
    // Create a channel, stub and make RPC calls (same as in the previous example)
    auto channel = grpc::CreateChannel(SCOPE, creds);
    std::unique_ptr<Speech::Stub> stub(Speech::NewStub(channel));
    grpc::Status s = stub->StreamingRecognize(requestStream(stop_audio), DEADLINE_SECS);
    // listenPrintLoop(stub.StreamingRecognize( request_stream(stop_audio), DEADLINE_SECS) );


    if (s.ok()) {
        // cout << "Status returned OK\nResponse Size :" << response.results_size() << endl;
        int a = 0;
        StreamingRecognitionResult results = response.results(0);
        SpeechRecognitionAlternative alternative = results.alternatives(a);
        cout << "Response :" << alternative.transcript() << endl;
    } else if (s.ok()){
        cout << "Status Returned Canceled" << endl;
    }else {
      cout << s.error_code() << ": " << s.error_message() << s.ok() << endl;
      cerr << "RPC failed" << endl;;
    }

    cout << "\nAll Finished!" << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

//////////////////
// SET UP OAUTH //
//////////////////
