# v1Beta1_gRPC_API

C++ implmentations using Google's Speech (v1Beta1) API. Checkout Google's [Speech API](https://cloud.google.com/speech/)

These projects also use  gRPC wich you can checkout at their [Main Website](http://www.grpc.io/) or the [Git Repo](https://github.com/grpc/grpc)

AudioSyncRequest and StreamingSyncRequest use SFML wich can be found here: [Main Website](http://www.sfml-dev.org/index.php) and [Git Repo](https://github.com/SFML/SFML)



Pro Tip: If you are running glorious Arch Linux you can install gRPC (wich includes protobuf3) from AUR

	$ yaourt -S grpc




### Projects

[BasicSyncRequest](/src/BasicSyncRequest/):
	

[AudioSyncRequest](/src/AudioSyncRequest/):

 This requires that you have SFML 2.4.0 installed (see http://www.sfml-dev.org/download.php for details)

 Pro Tip: If you are running glorious Arch Linux you can download it from pacman

	$ sudo pacman -S sfml


[StreamingRequest](/src/StreamingRequest/):

 Currently not working, check out AudioSyncRequest for the closest thing to streaming




### Notes:
 
 I have another repo [VisualAudio](https://github.com/teganburns/Visual_Audio) that uses SFML to Graphicaly display the audio input. I plan to include in my final Speech API project along with 
StreamingRequest. Go check it out it's 
pretty neat!! :)
