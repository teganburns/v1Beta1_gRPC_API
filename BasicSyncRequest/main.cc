#include <iostream>
#include <unistd.h>
#include <fstream>
#include <typeinfo>

using namespace std;

#include <grpc++/grpc++.h>
#include "cloud_speech.grpc.pb.h"

// Uncomment line below to enable debugging //
//#define DEBUG

int main( int argc, char* argv[] ) {

    //system("clear");

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // VERIFY  ARGS //
     if (argc < 2) {
    	cerr << "Usage:  " << argv[0] << " AUDIO_FILE[Required]" << endl;
    	return -1;
    }


    //--------------//
    /// SET UP SRR ///
    //-------------//
    string line = "-----------------------------------------------------------------";
    string SCOPE = "speech.googleapis.com";

    google::cloud::speech::v1beta1::SyncRecognizeRequest sync_recognize_request;
    google::cloud::speech::v1beta1::RecognitionConfig config;
    google::cloud::speech::v1beta1::RecognitionAudio audio;
    google::cloud::speech::v1beta1::SyncRecognizeResponse response;
    google::cloud::speech::v1beta1::SpeechRecognitionResult results;
    google::cloud::speech::v1beta1::SpeechRecognitionAlternative alternative;

    grpc::Status status;
    grpc::ClientContext context;

    // CONFIG IS HERE //
    sync_recognize_request.set_allocated_config( &config );
    config.set_encoding( google::cloud::speech::v1beta1::RecognitionConfig_AudioEncoding::RecognitionConfig_AudioEncoding_FLAC );
    config.set_sample_rate( static_cast<google::protobuf::int32>( 16000 )  );
    config.set_max_alternatives( static_cast<google::protobuf::int32>( 1 ) );
    config.set_profanity_filter( false );

    cout << "Encoding: " << config.encoding() << endl;

    // DISPLAY RESULTS FOR DEBUGGING //
    #ifdef DEBUG
        cout << "\n--Quick config results--" << "\nEncoding: " << config.encoding() << "\nSample Rate: " << config.sample_rate() << "\nMax Alternatives: " << config.max_alternatives() << "\nSet Profanity Filter: " << config.profanity_filter() << endl;
    #endif

    //--------------//
    /// LOAD AUDIO ///
    //-------------//

    // LOAD AUDIO FILE DIRECTLY INTO STRING //
    ifstream infile;
    infile.open( argv[1] );
    string buf { istreambuf_iterator<char>( infile ), istreambuf_iterator<char>() };
    infile.close();

    // SET AUDIO //
    audio.set_content( buf );
    sync_recognize_request.set_allocated_audio( &audio );


    //---------------------------------//
    /// OPEN CHANNEL AND SEND REQUEST ///
    //--------------------------------//

    cout << "Attempting to send request to server" << endl;

    // ALLOCATE MEMORY FOR RESPONSES (or something...)//


    // Open Channel with Google's Server //
    auto creds = grpc::GoogleDefaultCredentials();
    // Create a channel, stub and make RPC calls //
    auto channel = grpc::CreateChannel( SCOPE, creds );
    std::unique_ptr< google::cloud::speech::v1beta1::Speech::Stub> stub( google::cloud::speech::v1beta1::Speech::NewStub( channel ) );
    status = stub->SyncRecognize(&context, sync_recognize_request, &response);

    cout << "status.ok(): " << status.ok() << endl;

    if ( status.ok() ) {
        cout << "Status returned OK\nResponse Size :" << response.results_size() << endl;
        int a = 0;
        results = response.results( 0 );
        alternative = results.alternatives( a );
        cout << "Response :" << alternative.transcript() << endl;

    } else if ( status.ok() ){
        cout << "Status Returned Canceled" << endl;

    }else {
      cout << status.error_code() << ": " << status.error_message() << status.ok() << endl;
      cerr << "RPC failed" << endl;;

    }

    cout << "\nAll Finished!" << endl;

    //google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
