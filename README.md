# v1Beta1_gRPC_API

C++ implmentations using Google's Speech (v1Beta1) API. Checkout Google's Speech API [here](https://cloud.google.com/speech/) and gRPC wich can be found here: [Main Website](http://www.grpc.io/) and the
[Git Repo](https://github.com/grpc/grpc)

AudioSyncRequest and StreamingSyncRequest use SFML wich can be found here: [Main Website](http://www.sfml-dev.org/index.php) and [Git Repo](https://github.com/SFML/SFML)


You gon need to install protobuf and gRPC [https://github.com/grpc/grpc] https://grpc.io
	If you are running glorious Arch Linux you can install gRPC (wich includes protobuf3) from AUR


[BasicSyncRequest](/src/BasicSyncRequest/):
	
	-
	

[AudioSyncRequest](/src/AudioSyncRequest/):

	- This requires that you have SFML 2.4.0 installed (see http://www.sfml-dev.org/download.php for details)
		If you are running Arch you can download it from pacman

[StreamingRequest](/src/StreamingRequest/):

	- Currently not working, check out AudioSyncRequest for the closest thing to streaming


