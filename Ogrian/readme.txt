Ogrian Carpet v0.6
http://ogrian.sf.net
======================

license
-----
Ogrian Carpet is licenced under the GPL. This means that it is Free Software. You are free to modify and redistribute it as long as you obey the terms of the GPL. The full terms of the GPL can be seen at http://www.gnu.org/copyleft/gpl.html

controls
-----
w - move forward
s - move backwards
a - move left
d - move right
left mouse button - cast a fireball
right mouse button - cast the claim spell
middle mouse button or left control - build castle or tower
escape - show/hide menu
F1 - show/hide framerate, etc.
TAB - show players and mana amounts

how to play
-----
The object of the game is to build a castle, collect mana, and destroy your enemies. To build a castle, select a location, look at the ground, and press the middle mouse button or the left control key. Note, you cannot build castles very close to water or other castles. Your castle starts out small, with one baloon and one kamikazi crane. As you claim mana, your baloon will collect the claimed mana and bring it back to your castle. As more mana is added to your castle, it will grow larger. New baloons and cranes will also be created automatically as your castle grows. 

Basically, the game consists of fighting for control of mana. Whenever you encounter another wizard, shoot them with fireballs. If you hit them enough, the will "die" and be sent back to their castle. You are then free to claim all the mana in the area for yourself. Once all the mana has been claimed, attack your enemy's castle to get mana out of it so you can claim it for yourself. Once you enemy's castle is out of mana, you can eliminate it. If you kill your enemy while his castle is gone, he is banished and no longer in the game. When all of your opponents have been banished, you've won. When a player has been banished, they may continue to observe the game, and it is possible for their castle to be rebuilt if it receives more mana. (though they will not be un-banished)

Things
-----
* Mana are the semi-transparent bubble-like circles. They start out unclaimed and gray. To claim them for your own, cast your "claim" spell at them by pressing the right mouse button. If your claim spell hits the mana, it will change colour to match the colour of your team. Then, one of your baloons will come and pick it up and then take it back to your castle. Whenever you see mana that is not your colour, claim it for yourself! Be aware that mana slowly rolls downhill, and if two mana collide, they form a bigger mana. 

* Baloons are automatic mana collectors. They cannot be harmed by any form of attack. The only way to foil them is to claim for yourself the mana that they are collecting. 

* Kamikazi Cranes (as in origami cranes) defend castles. They fly around the castle looking for enemies. When they see one, they fly towards it. If they collide with their target, they explode, dealing damage. They then respawn and start the process all over again. If you see a crane flying towards you, the only way to avoid taking damage is to kill it before it hits you.

Spells
-----
* Fireballs are the sole offensive spell. To cast one, simply press the left mouse button. You can cast as many as you like. You'll never run out. You can also simply hold down the left mouse button to fire them continuously. Note, they fall a bit when they fly, so aim a bit above your target. The fireball has more range then the other spells. 

* The Claim spell allows you to claim mana. Press the right mouse button to cast it. It flies straight, but doesn't have much range.

* The Build spell allows you to build a castle. Once you have build a castle, this spell will build a free-standing tower anywhere on the map (except very near water or other buildings).

Buildings
-----
* Castles store mana. The more yours has, the bigger it gets. (but it stops growing at 400 mana) It starts with one baloon and one crane, but as it grows, it makes more. As a castle takes damage, it shrinks and drops mana into the surrounding area. If it shrinks too much, it will lose baloons and cranes. Once a castle is out of mana, further damage will reduce it to rubble. Once it is reduced to rubble (when the last piece is almost down to the ground and won't go down any further), the wizard who owns that castle can be banished (eliminated from the game) by killing him

* Towers are outposts that can be built anywhere (except very near water or other buildings). You may build as many towers as you can afford (they cost 100 mana each - that's 100 mana that is removed from your castle, shrinking it). Each tower has two cranes that patrol the area. Towers also claim nearby mana automatically. Strategic use of towers is probably the key to victory. You can unbuild towers by hitting them with the build spell after they have been constructed. If a tower is destroyed, it will drop the 100 mana it took to build it. If it is unbuilt, the mana will return directly to its castle. 

Game Modes
----
skirmish - this is the single-player option. It loads the selected map (the list on the left) and loads 5 computer-controlled opponents, each with a tower. Destroy their towers and claim mana. There is no way to really win - since they have no castles, they cannot be banished. However, they dont claim mana (only their towers do), so you can practice accumulating mana and building towers. 

Host - This starts a server. Up to three other players can connect to you. Once they are all connected, bring up the menu with ESC and then press the "Start Game" button. Before you start the game, no mana can be claimed and no castles can be built. When the game is started, each player will be placed at a corner of the map and the fight can begin. 

Join - this joins the server that you have listed in your config.txt file.

Before playing a multiplayer game, it is important to configure your character. Edit config.txt to set your name, your colour, and the IP address of the server you wish to connect to. You may also set other options in this file. 

To find the ip address of the host machine run "ipconfig" at a command prompt. To get a command prompt, select Start->Run and type "cmd". The ip address is in the form of "192.168.1.1"


Credits
==========
programming and design:
 Mike Prosser

music: (Riding the Dragon, Last Glimpse)
 Adrian Ouarar

skins:
 Jeff Knous
 Mike Prosser
 Rick Prosser
 Jess Prosser

sounds and tree sprites:
 WorldForge 

libraries:
 Ogre3D
 FMOD
 RakNet