# Mappings Helper
# (Just an example of how it could work)
# Uses some heuristics to compute which buttons on GWR are easier to press then others
# Prioritizes keys that are used more often

frequency_list: list[str] = [
    "E",
    "T",
    "A",
    "O",
    "I",
    "N",
    "S",
    "H",
    "R",
    "L",
    "D",
    "C",
    "U",
    "M",
    "W",
    "F",
    "G",
    "Y",
    "P",
    "B",
    "V",
    "K",
    "J",
    "X",
    "Q",
    "Z",
]

buttons_id = {
    "C_tip":  0,
    "C_fren": 1,
    "C_mid":  2,
    "L_lnear": 3,
    "R_rnear": 4,
    "L_lfar": 5,
    "R_rfar": 6,
}
# how hard it is to push the buttons
badness = [4, 1, 3, 2, 2, 5, 5]

buttons_id_extended = {
    "C_tip": 0,
    "C_fren": 1,
    "C_mid": 2,
    "L_lnear": 3,
    "R_rnear": 4,
    "L_lfar": 5,
    "R_rfar": 6,
    "C_low": 7,
    "C_top": 8,
}
# how hard it is to push the buttons
badness_extended = [4, 1, 3, 2, 2, 5, 5, 7, 7]

# Other notes:
# PCB and GPIO exapander mapping
#  name     id    gpio  pcb marking
# C_tip:    0, ->  A7      H    
# C_fren:   1, ->  A3      D
# C_mid:    2, ->  A1      B
# L_lnear:  3, ->  A4      E
# R_rnear:  4, ->  A6      G
# L_lfar:   5, ->  A0      A
# R_rfar:   6, ->  A5      C

# non alphabet chars reserve a combo here
reserved_binds = {
    "C_mid_C_mid": "space",
    "C_mid_C_tip": "backspace",
    "L_lnear_L_lfar": "exclaim",
    "L_lnear_R_rfar": "question",
    "C_tip_C_tip": "period",
    "R_rnear_R_rfar": "enter",
}

def __main__():
    two_button_combos: list[tuple[str, int]] = []
    for a in buttons_id:
        for b in buttons_id:
            score = (badness[buttons_id[a]] + badness[buttons_id[b]]) * 2
            if a == b:
                score -= 2 # pressing same button twice is easier
            if (a[0] == 'L' and b[0] == 'R') or (a[0] == 'R' and b[0] == 'L'):
                score += 1 # pressing buttons on opposite sides is harder
            
            two_button_combos.append((a + ':' + b, score))


    combos_sorted = sorted(two_button_combos, key=lambda x: x[1])

    for letter in frequency_list:
        next_available_combo = combos_sorted[0]
        next_available_name = next_available_combo[0]
        while next_available_name in reserved_binds:
            combos_sorted.remove(next_available_combo)
            next_available_combo = combos_sorted[0]
            next_available_name = next_available_combo[0]
        
        print(f"{letter}: {next_available_name} ({buttons_id[next_available_name.split(':')[0]]}-{buttons_id[next_available_name.split(':')[1]]})")
        combos_sorted.remove(next_available_combo)

if __name__ == "__main__":
    __main__()
