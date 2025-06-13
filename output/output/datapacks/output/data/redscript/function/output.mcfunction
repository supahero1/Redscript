scoreboard objectives add cmp0 dummy "cmp0"
scoreboard objectives add cmp1 dummy "cmp1"
scoreboard objectives add cmp2 dummy "cmp2"
scoreboard objectives add r0 dummy "0"
scoreboard objectives add r1 dummy "1"
execute if score _CPU cmp0 matches 1 run data merge storage redscript:_program {"variables":[], "registers":[], "_internal":{}, "stack":[], "ret": 0, "temp": 0}
execute if score _CPU cmp0 matches 1 run scoreboard objectives add temp dummy "temp"
data modify storage redscript:_program variables append value {"value":12,"scope":1,"type":0}
function redscript:return_if_even
data remove storage redscript:_program variables[0]
