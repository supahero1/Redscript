data modify redscript:_program variables append value {value:4,scope:0,type:1}
data modify redscript:_program variables append value {value:0,scope:0,type:1}
data modify storage redscript:_program variables[1].value set from storage redscript:_program variables[0].value
execute store result score _CPU 0 run data get redscript:_program variables[0].value
scoreboard players _CPU 0 add 2
data modify redscript:_program variables append value {value:0,scope:0,type:1}
execute store result redscript:_program variables[2].value int 1 run scoreboard players get _CPU 0
