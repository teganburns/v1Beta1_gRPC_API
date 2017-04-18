**UPDATED: Cloud Speech is now generally avaiable and the docs have been updated. I'll be looking into the docs and updateing to reflect any changes to the stable version within the week-ish. [REF](https://cloudplatform.googleblog.com/2017/04/Cloud-Speech-API-is-now-generally-available.html)**

# v1Beta1_gRPC_API


C++ implmentations using Google's Speech (v1Beta1) API. Checkout Google's [Speech API](https://cloud.google.com/speech/)

ALL projects  use  gRPC wich you can checkout at their [Main Website](http://www.grpc.io/) or the [Git Repo](https://github.com/grpc/grpc)

AudioSyncRequest and StreamingSyncRequest use SFML wich can be found here: [Main Website](http://www.sfml-dev.org/index.php) and [Git Repo](https://github.com/SFML/SFML)

---

Pro Tip: If you are running glorious Arch Linux you can install gRPC (wich includes protobuf3) from AUR

	$ yaourt -S grpc
---



## Projects

[BasicSyncRequest](/BasicSyncRequest/):
	

[AudioSyncRequest](/AudioSyncRequest/):

 This requires that you have SFML 2.4.0 installed (see http://www.sfml-dev.org/download.php for details)

 Pro Tip: If you are running glorious Arch Linux you can download it from pacman

	$ sudo pacman -S sfml


[StreamingRequest](/StreamingRequest/):

 Working but it's not pretty...
 I have added comments to try and clarify but would suggest you get started with the Basic SyncRequest first.

 This requires that you have SFML 2.4.0 installed (see http://www.sfml-dev.org/download.php for details)

 Pro Tip: If you are running glorious Arch Linux you can download it from pacman

	$ sudo pacman -S sfml




### Notes:

 I have another repo [VisualAudio](https://github.com/teganburns/Visual_Audio) that uses SFML to Graphicaly display the audio input. I plan to include in my final Speech API project along with 
StreamingRequest. Go check it out it's pretty neat!! :)
