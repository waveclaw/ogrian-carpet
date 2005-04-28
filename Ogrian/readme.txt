Ogrian Carpet v0.8
http://ogrian.sf.net
======================

license
-----
Ogrian Carpet is licenced under the GPL. This means that it is Free Software. You are free to modify and redistribute it as long as you obey the terms of the GPL. The full terms of the GPL can be seen at http://www.gnu.org/copyleft/gpl.html
How to Play

controls
-----
w or up arrow - move forward
s or down arrow - move backwards
a or left arrow - move left
d or right arrow - move right
left mouse button - cast a spell
q or mousewheel up - select previous spell
e or mousewheel down - select next spell
0-9 - select a spell directly
escape - show/hide menu
F1 - show/hide framerate, etc.
TAB - show players and mana amounts

how to play
-----
The object of the game is to build a castle, collect mana, and destroy your enemies. To build a castle, select a location, look at the ground, and cast the build spell. Note, you cannot build castles very close to water or other castles. Your castle starts out small, with only one turret. As more mana is added to your castle, it will gain more turrets. Each turret adds another crane to your castle's defense and another spell to your arsenal. 

Basically, the game consists of fighting for control of mana. Whenever you encounter another wizard, shoot them with fireballs. If you hit them enough, they will "die" and be sent back to their castle. You are then free to claim all the mana in the area for yourself. Once all the mana has been claimed, attack your enemy's castle to get mana out of it so you can claim it for yourself. Once you enemy's castle is out of mana, you can eliminate it by killing it's heart. When all of your opponents have been banished, you've won.

Things
-----
Mana are the semi-transparent bubble-like circles. They start out unclaimed and white. To claim them for your own, cast your "claim" spell at them. If your claim spell hits the mana, it will change colour to match the colour of your team. Then, it will slowly migrate towards your castle. When it reaches your castle, it is absorbed and your maximum mana (base mana) goes up. Whenever you see mana that is not your colour, claim it for yourself! Be aware that unclaimed mana slowly rolls downhill, and if two mana collide, they form a bigger mana.

Kamikazi Cranes (as in origami cranes) defend castles and towers. They fly around the castle looking for enemies. When they see one, they fly towards it. If they collide with their target, they explode, dealing damage. They then respawn and start the process all over again. If you see a crane flying towards you, the only way to avoid taking damage is to kill it before it hits you.

Sentinels are stationary gaurdians that fire fireballs at nearby enemies.

Gnomes are like sentinels, but they follow their wizard around.

Giant Ticks are monsters that attack enemies by jumping at them. If they touch an enemy, it is dealt a lot of damage. Ticks are very effective at killing castles and towers.

Note: Things are tinted to show which team they are on.

Spells
-----

The spells that you have are shown at the top of the screen:

The Claim spell allows you to claim mana and huts. It also heals your monsters.

The Build spell allows you to build a castle. Once you have build a castle, this spell will build a free-standing tower anywhere on the map (except very near water or other buildings). If you cast it on an allies castle, some of your mana will be transfered to them.

Fireball* is the primary offensive spell. Note, they fall a bit as they fly, so aim a bit above your target. As you gain more mana, this will automatically upgrade to "akimbo fireball" and then to "Firestorm".

Teleport takes you and all of your ticks and gnomes back to your castle.

Summon Sentinel creates a sentinel.

Summon Gnome creates a gnome that will follow you around.

Speed* makes you go fast.

Summon Tick creates a tick that will follow you around.

Albatross is a giant crane that flies like a missile, dealing lots of damage to whatever it hits.

* for best results, don't just tap the mouse button, hold it down!

Buildings
-----
Castles store mana. The more yours has, the bigger it gets. (but it stops growing at 400 mana) It starts with one turret, but as it grows, it makes more. As a castle takes damage, it shrinks and drops mana into the surrounding area. Once a castle is out of mana, further damage will expose the castle's heart. If the castle's heart is destroyed, that player is banished from the game (though they may still spectate as a ghost). 

Towers are outposts that can be built anywhere (except very near water or other buildings). You may build as many towers as you can afford (they cost 75 of your max mana (base mana) each - that's 75 mana that is removed from your castle, shrinking it). Each tower has two cranes that patrol the area. Towers also claim nearby mana automatically. Strategic use of towers is probably the key to victory. You can unbuild towers by hitting them with the build spell after they have been constructed. If a tower is destroyed, it will drop the 100 mana it took to build it. If it is unbuilt, the mana will return directly to its castle. Towers and castles also have a portal that teleport you to the next tower. 

Mana Shrines are scattered around the map randomly at the start of the game. They can be claimed like mana to increase your mana regeneration rate.

Game Modes
----
Skirmish - this is the single-player option. It loads the selected map (the list on the left) and loads lots of monsters and towers. Destroy them and collect the mana. Theres no way to actually win, however. This is just meant for practice.

Host - This starts a server. Up to three other players can connect to you. Once they are all connected, bring up the menu with ESC and then press the "Start Game" button. Before you start the game, no mana can be claimed and no castles can be built. When the game is started, each player will be placed at a start location and the fight can begin.

Join - this joins a server. Type in the name or IP address of the server to connect to it.

To find the ip address of the host machine run "ipconfig" at a command prompt. To get a command prompt, select Start->Run and type "cmd". The ip address is in the form of "192.168.1.1"

Before playing a multiplayer game, it is important to configure your character with the Configure menu.

Also note, players with the same colour are considered "allies". Their monsters will not attack each other, and towers will not claim each other's mana. Also, you can give your ally mana by casting the build spell on their castle. 

Credits
---
programming and design:
-Mike Prosser

castle and tower models/textures:
-Carl Olsson
-John Michel

mana shrine model/texture:
-Chris Browning

music: 
-Adrian Ouarar via WorldForge
-John Michel

skins:
-Jeff Knous
-Jess Prosser
-Mike Prosser
-Rick Prosser

sprites:
-Mike Prosser
-Jeremiah "Waveclaw" Powell
-WorldForge
-John Michel

sounds:
-Mike Prosser
-WorldForge

maps:
-Mike Prosser
-Corey Ulmer
-John Michel

libraries:
-Ogre3D
-FMOD
-RakNet
