execute store result score _CPU r0 run data get storage redscript:_program variables[0].value
scoreboard players set _CPU r1 4
scoreboard players operation _CPU r0 *= _CPU r1
scoreboard players set _CPU cmp0 0
execute if score _CPU r0 matches 16 run scoreboard players set _CPU cmp0 1
execute if score _CPU cmp0 matches 1 run function redscript:_do_stuff
return 0
