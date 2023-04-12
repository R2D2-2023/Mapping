# Mapping
Git repository voor de mapping groep (positie bepaling 1)

Jesse branch

lidar_tutorial.cpp bevat minimale ydlidar code

visualDemo.cpp was oude code, deze kon manual slam doen

scanExporter.cpp is nieuwste code, export images naar .pngs

averagingRotation.c was test file om averaging methode te testen

blurShader.fs is fragment shader die gaussian blur applied, wordt alleen gebruikt door visualDemo.cpp


Om de positioneel team te helpen, om te helpen visualeren en techniquen proberen zijn er meerde manual scans gemaakt.
Deze bestaan uit meerde lossen plaatjes bij elkaar gezet met behulp van een photoshop programma.

De eerste scan was een proof of concept maar er waren meerde problemen mee, de lidar was gedraait tijdens het scannen en de scans waren te ver van elkaar gemaakt, dit maakte het zeer moeilijk om het handmatig in elkaar te zetten, ondanks dat de gehele verdieping werd gescanned kon ik maar 2 hallen voor elkaar krijgen.
![manual_scan_1](https://user-images.githubusercontent.com/50572621/231447478-23650931-1783-4f6e-86be-f9163c4f067e.png)

De tweede scan ging een stuk beter toen ik realizeerde wat mis was met de eerste. Nu gingen we maar door een gang en hebben geprobeerd om de lidar recht vooruit te houden en we maakte scans veel vaker. Dit maakte het veel makkelijker om scan manually allemaal bij elkaar te zetten. Dit laat zien hoe belangerijk het is om in ieder geval rotatie niet meer te hoeven gokken.
Vandaar dat er zo veel moeite is gestopt in de magnetometers werkend proberen te krijgen, deze zouden ervoor zorgen dat de rotatie van de scan en de map altijd het zelfde zijn.
![manual_scan_2](https://user-images.githubusercontent.com/50572621/231448289-822d6f8f-195a-46cc-b7f7-086ab1bc6f10.png)

Voor scan 3 realizeerde ik me dat we de lidar data anders konden renderen. We konden aanemen dat alles tussen de lidar en het punt dat het raakt leeg is en we kunnen dat intekenen met wit. Dit zorgte voor redelijk wat noise, ondanks dat de intensity die van de lidar wordt terug gegeven voor ieder punt werd gebruikt om sommige lijnen meer doorzichtig te maken als de lidar niet 100% zeker was wat er gaande was is dit niet genoeg geweest om een goede image te krijgen.
![manual_scan_3](https://user-images.githubusercontent.com/50572621/231448848-ca530b41-79fb-4994-972b-4ae3a857262d.png)

Met 2 simpele fixen konden we dit groten deels fixen, alle punten verder dan MAX_DISTANCE (7 meter) werden geskipped. Dit fixte al wat vreemde lijnen, de tweede was om langere lijnen meer doorzichtig te maken. De lidar wordt minder precies hoe verder het gaat, vandaar dat dit zorgt voor een rustige map met minder noise.
![manual_scan_4](https://user-images.githubusercontent.com/50572621/231450113-3c5b0de9-7976-44b4-b9b2-b201ec1d22fe.png)

Deze scan is nog steeds niet perfect, de groene punten aan het uiteinde worden over getekenend met nieuwe lijnen, deze kunnen beter later worden getekened om dit te vermijden. De witte gedeeltes zijn ook zeer inconsistant, vol met gaten en onzekere lijnen.
