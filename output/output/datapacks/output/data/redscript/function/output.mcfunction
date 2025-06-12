data merge storage redscript:_program {"variables":[], "registers":[], "_internal":{}, "stack":[], "temp": 0}
scoreboard objectives add temp dummy "temp"
scoreboard objectives add r0 dummy "0"
scoreboard objectives add r1 dummy "1"
scoreboard objectives add r2 dummy "2"
scoreboard objectives add r3 dummy "3"
scoreboard objectives add r4 dummy "4"
scoreboard objectives add r5 dummy "5"
scoreboard objectives add r6 dummy "6"
scoreboard objectives add r7 dummy "7"
scoreboard objectives add r8 dummy "8"
scoreboard objectives add r9 dummy "9"
data modify storage redscript:_program variables append value {"value":4,"scope":0,"type":1}
execute store result score _CPU r0 run data get storage redscript:_program variables[0].value
scoreboard players set _CPU r1 2
scoreboard players operation _CPU r0 %= _CPU r1
data players set _CPU temp 0
scoreboard players set _CPU cmp0 0
execute if score players get _CPU r0 = _CPU temp run players set _CPU cmp0 1
execute if score _CPU cmp0 = 1 tellraw @r "X is even."
execute unless score _CPU cmp0 = 1 tellraw @r "X is odd."
