SPRITE  spr_player      "sprites/player.png"    2 2 FAST 16
SPRITE  spr_shoot       "sprites/shoot.png"     2 2 FAST 26
SPRITE  spr_enemy       "sprites/enemy.png"     2 2 FAST 5

IMAGE   bg_mountains    "backgrounds/mountains.png" BEST

TILESET world1_tiles    "worlds/world1/world1_tiles.png" BEST ALL
PALETTE world1_pal      "worlds/world1/world1_pal.pal"
MAP     world1_map      "worlds/world1/world1_map.tmx" map_layer BEST 0

OBJECTS  world1_enemies      "worlds/world1/world1_map.tmx" enemy_layer "x:f32;y:f32;room:u8"