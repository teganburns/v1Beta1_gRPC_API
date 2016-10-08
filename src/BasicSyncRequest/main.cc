#include <iostream>
#include <unistd.h>
#include <fstream>

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
using google::cloud::speech::v1beta1::Speech;

string line = "-----------------------------------------------------------------";
string SCOPE = "speech.googleapis.com";


int main(int argc, char* argv[])
{
    system("clear");

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // VERIFY  ARGS //
     if (argc < 2) {
    	cerr << "Usage:  " << argv[0] << " AUDIO_FILE[Required]" << endl;
    	return -1;
    }


    //--------------//
    /// SET UP SRR ///
    //-------------//
    SyncRecognizeRequest sync_recognize_request;
    RecognitionConfig config;

    sync_recognize_request.set_allocated_config(&config);

    // CONFIG IS HERE //
    ::google::protobuf::int32 a = 16000;
    ::google::protobuf::int32 b = 1;
    google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding encoding = google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_FLAC;

    config.set_encoding(encoding);
    config.set_sample_rate(a);
    config.set_max_alternatives(b);
    config.set_profanity_filter(false);

    // DISPLAY RESULTS FOR DEBUGGING //
    // cout << "\n--Quick config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;




    //--------------//
    /// LOAD AUDIO ///
    //-------------//

    // LOAD AUDIO FILE DIRECTLY INTO STRING //
    ifstream infile;
    infile.open(argv[1]);
    string buf { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    infile.close();

    // SET AUDIO //
    RecognitionAudio audio;
    audio.set_content(buf);
    sync_recognize_request.set_allocated_audio(&audio);


    //---------------------------------//
    /// OPEN CHANNEL AND SEND REQUEST ///
    //--------------------------------//

    cout << "Attempting to send request to server" << endl;

    // ALLOCATE MEMORY FOR RESPONSES (or something...)//

    grpc::ClientContext context;
    SyncRecognizeResponse response;

    // Open Channel with Google's Server //
    auto creds = grpc::GoogleDefaultCredentials();
    // Create a channel, stub and make RPC calls //
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

    cout << "\nAll Finished!" << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
