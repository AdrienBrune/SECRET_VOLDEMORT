## Description

<ul>
<li>Creator : BRUNE Adrien</li>
<li>Version : 1.0.0</li>
<li>Date : 29/04/2021</li>
</ul>

<img src="./SECRET_VOLDEMORT_GAME/Developpement/ressources/Secret_Voldemort_Cover.png" alt="Logo" width="200" height="300">

## Game

<p>Secret Voldemort is a strategic game playable from 5 up to 10 players. 
The games grants you a role that define your behaviour during the game.
The realm of magic is threaten by the dark forces of deatheaters lead by Voldemort himself.
If you join the deatheaters, try to take the power by electing Voldemort as Minister of magic.
But if you're the resistance well known as the Phenix order, you will have to fight the deatheaters by slaying them all, or their chief.
When the game starts you are alone, you will have to be perceptive enought to catch players of your team and rallying them to your cause.
By doing so, maybe you will be able to overcome the manipulation and be able to win the game, good luck.

Official Secret Hitler rules : https://www.secrethitler.com/assets/Secret_Hitler_Rules.pdf  
Video rules : https://www.youtube.com/watch?v=mbGXIDYdtas&ab_channel=TheRulesGirl

## Preview
<img src="./Preview/img1.png" alt="Logo" width="800" height="400">
<img src="./Preview/img2.png" alt="Logo" width="800" height="400">
<img src="./Preview/img3.png" alt="Logo" width="800" height="400">
<img src="./Preview/img4.png" alt="Logo" width="800" height="400">
<img src="./Preview/img5.png" alt="Logo" width="800" height="400">

## Technical point
* Only one serveur is needed to play.
* The game is using the Ethernet port 30000 to communicate.
* The server owner needs to operate a port redirection.
  * Go to your box configuration.
  * Got to port redirection.
  * Add a rule.
  * set TCP protocol.
  * set the range port to [30000, 30000] (single port 300000 is used).
  * set the computer IP adresse (go to Windows CMD, then tape the command line "ipconfig").
  * Then you will have to give your public IP to other player (find it on internet).
  * Be ready to play.
