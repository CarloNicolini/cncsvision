Questa cartella è divisa a seconda di esperimenti con deformazione costante o deformazione variabile.

CONTENUTI DELLE CARTELLE in ordine
************************************************************************************************
1) rotatingProbe
La cartella rotatingProbe esperimenti passivi con probeRotante (risposta continua)

PROBE CONTINUO \theta = 0:1:360
PROBE DISCRETO \theta={-1,1}

a)  rotatingProbe/demoWarrenPassiveOnlyTranslation.cpp
    Lo stimolo si muove solidale al piano di proiezione in orizzontale e verticale. L'occhio segue la posizione dello stimolo, in modo da evitare lo shear.
    In questa versione il probe è CONTINUO
    -> File input:  parametersPassive.txt

b)   rotatingProbe/demoWarrenPassiveOnlyTranslationOccluded.cpp
    Lo stimolo si muove solidale al piano di proiezione orizzontale e verticale. L'occhio segue la posizione dello stimolo, in modo da evitare lo shear.
    In questa versione il probe è CONTINUO.

c)  rotatingProbe/demoWarrenPassiveRotation.cpp
    -> Lo stimolo si muove ruotando e traslando in modo da mostrare la stessa faccia all'osservatore. No correzione per shear.
    -> File input: parametersPassive.txt
	
************************************************************************************************

2) constantDef/
Questa cartella contiene esperimenti per cui  la def dei punti sul piano è resa costante dalla soluzione dell'equazione differenziale
		\frac{d \theta(t)}{dt} = \frac{K}{\tan( \theta(t) )
Tutti gli esperimenti assumono probe DISCRETO.
La soluzione è implementata frame per frame all'interno della drawRedDotPlane
Occlusore cilindrico impostabile dalla variable DrawOccluder nel file parametersPassiveCdef.txt"
Dimensione dello stimolo quadrato 5x5 senza occlusore, circolare 5 cm raggio con occlusore 
a)   demoWarrenPassiveCdefNoProbe.cpp
    -> Lo stimolo si muove solidale al piano di proiezione orizzontale e verticale. L'occhio segue la posizione dello stimolo, in modo da evitare lo shear.
    -> In questa versione il probe è DISCRETO ed assume valori +1 o -1.
    -> File input: parametersPassiveCdef.txt
  
************************************************************************************************

3) varyingDef/

Questa cartella contiene esperimenti per cui  la def non è costante.
Tutti gli esperimenti assumono probe DISCRETO.

a)   demoWarrenPassiveOnlyTranslationNoProbe.cpp
    Lo stimolo si muove solidale al piano di proiezione orizzontale e verticale. L'occhio segue la posizione dello stimolo, in modo da evitare lo shear.
    In questa versione il probe è DISCRETO ed assume valori +1 o -1.
	Occlusore cilindrico ASSENTE
    -> File input: parametersPassive.txt
    -> File output: dataset/[BaseDir]/responseFile_[SubjectName].txt
    
b)   demoWarrenPassiveOnlyTranslationOccludedNoProbe.cpp
    Lo stimolo si muove solidale al piano di proiezione orizzontale e verticale. L'occhio segue la posizione dello stimolo, in modo da evitare lo shear.
    In questa versione il probe è DISCRETO ed assume valori +1 o -1
	Occlusore cilindrico PRESENTE
    -> File input: parametersPassive.txt
    -> File output: dataset/[BaseDir]/responseFile_[SubjectName].txt
	
************************************************************************************************

Come impostare i file dei parametri parametersPassive.txt
Qui spiegato il significato:

fStimulusDuration:  [n] Durata dello stimolo in millisecondi
fDef:       [n]     Deformazione del piano (numero compreso fra 0 e 1 solitamente)
fTilt:				[n]		Il Tilt del piano.
fSlant:			[n]		Lo slant del piano.
fAnchored:	[n]		L'asse lungo il quale lo stimolo si muove. 0= stimolo fermo al centro, 1 = stimolo su asse X, 2 = stimolo su asse Y.
					
fFollowingSpeed:[s1,s2,..]  Rappresenta il tempo di presentazione dello stimolo. Stimoli più veloci hanno t < 1 stimoli più lenti t> 1 in maniera direttamente proporzionale.
                            Per esempio, voglio fare una serie di stimoli lunghi 0.1, 0.5, 5 secondi l'uno, la sintassi da usare è:
                            fStimulusSpeed: 0.1 0.5 5

fRotationSpeed: [n]         Rappresenta il massimo angolo di rotazione dello stimolo intorno al suo asse. Lo stimolo ruota di n gradi in un secondo,
                            corrispondente ad una spazzata da destra a sinistra o viceversa quando StimulusSpeed=1.
                            Per esempio, voglio che lo stimolo faccia una rotazione di 20 gradi durante il tempo di presentazione, semplicemente inserire:
                            fRotationSpeed: 20
                            Per gli esperimenti a deformazione costante, rotation speed non rappresenta l'angolo massimo di rotazione ma la velocità angolare in gradi/sec
fOnset:         [n]         Direzione di entrata dello stimolo (può essere {1,-1})
