execute store result score _CPU r0 run data get storage redscript:_program variables[0].value
scoreboard players set _CPU r1 2
scoreboard players operation _CPU r0 %= _CPU r1
scoreboard players set _CPU cmp1 0
execute unless score _CPU r0 matches 0 run scoreboard players set _CPU cmp1 1
execute if score _CPU cmp1 matches 1 run store result score _CPU r0 run data get storage redscript:_program variables[0].value
execute if score _CPU cmp1 matches 1 run scoreboard players set _CPU r1 2
execute if score _CPU cmp1 matches 1 run scoreboard players operation _CPU r0 /= _CPU r1
execute if score _CPU cmp1 matches 1 run scoreboard players set _CPU cmp2 0
execute if score _CPU cmp1 matches 1 if score _CPU r0 matches 4 run scoreboard players set _CPU cmp2 1
execute if score _CPU cmp2 matches 1 run data modify storage _internal variables[0].value set value 2
execute if score _CPU cmp1 matches 1 run data modify storage redscript:_program redscript:_program ret set from storage redscript:_program redscript:_program variables[0].value
execute if score _CPU cmp1 matches 1 run return 1
execute unless score _CPU cmp1 matches 1 run return 0
