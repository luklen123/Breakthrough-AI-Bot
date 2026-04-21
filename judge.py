import subprocess
import time

p1 = subprocess.Popen(['./breakthrough', 'bot', 'W'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
p2 = subprocess.Popen(['./breakthrough', 'bot', 'B'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)

print("--- START TURNIEJU: BOT vs BOT ---")

runda = 1
while True:
    try:
        move_w = p1.stdout.readline()
        if not move_w:
            print("\nGra zakończona! (Białe zakończyły działanie)")
            break
            
        print(f"Runda {runda} | Białe zagrały: {move_w.strip()}")
        
        p2.stdin.write(move_w)
        p2.stdin.flush()

        time.sleep(0.3)

        move_b = p2.stdout.readline()
        if not move_b:
            print("\nGra zakończona! (Czarne zakończyły działanie)")
            break
            
        print(f"Runda {runda} | Czarne zagrały: {move_b.strip()}")
        
        p1.stdin.write(move_b)
        p1.stdin.flush()
        
        runda += 1

    except BrokenPipeError:
        print("\n==============================")
        print("Gra zakończona! (Złamane połączenie - program osiągnął stan końcowy)")
        print("==============================")
        break