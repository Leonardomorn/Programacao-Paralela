import subprocess
import time

# Abra um arquivo para escrever a saída concatenada
with open('saida_paralela_2threads15.txt', 'w') as output_file:
    output_file.write("Distancia Tempo\n")
    # Itere 20 vezes
    for i in range(20):
        # Execute o programa C
        process = subprocess.Popen(["mpirun", "--hostfile", "hosts.txt", "-np", "2", "./tsp"], stdin=open("entrada15", "r"), stdout=subprocess.PIPE, text=True)
        
        # Obtenha a saída
        output, _ = process.communicate()
        
        # Escreva a saída no arquivo
        output_file.write(output)
        print("execução " + str(i) + " escrita no arquivo")
        # Aguarde até o processo terminar
        process.wait()

        time.sleep(30)
