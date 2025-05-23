import copy


my_moves = {}
engine_moves = {}

with open("moves.txt", "r") as f:
    data = f.readlines()

toggle = 0

for move in data:
    if move[0] == "-":
        toggle = 1
        continue
    
    end = move.find(":")
    mv = move[0 : end]

    end2 = move.find(r"\n")
    count = move[end+1 : end2]

    if toggle == 0:
        my_moves[mv] = int(count)
    else:
        engine_moves[mv] = int(count)

to_use = 0
to_remain = 0
string_to_remain = ""
if len(my_moves) > len(engine_moves):
    to_use = engine_moves
    to_remain = my_moves
    string_to_remain = "my_moves"
else:
    to_use = my_moves
    to_remain = engine_moves
    string_to_remain = "engine_moves"

copy_to_use = copy.copy(to_use)
for move in copy_to_use.keys():
    print(f"{move}: {engine_moves[move]-my_moves[move]}")
    engine_moves.pop(move)
    my_moves.pop(move)

print(f"Missing Moves From: {to_use}")
for move in to_remain.keys():
    print(f"{move}")