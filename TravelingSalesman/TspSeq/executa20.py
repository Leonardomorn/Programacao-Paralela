import subprocess
import time


# Abra um arquivo para escrever a saída concatenada
with open('saida_sequencial15.txt', 'w') as output_file:
    output_file.write("Distancia sequencial paralelo total\n")
    # Itere 20 vezes
    for i in range(20):
        # Execute o programa C
        process = subprocess.Popen("./tsp", stdin=open("entrada15", "r"), stdout=subprocess.PIPE, text=True)
        
        # Obtenha a saída
        output, _ = process.communicate()
        
        # Escreva a saída no arquivo
        output_file.write(output)
        print("execução " + str(i) + " escrita no arquivo")
        # Aguarde até o processo terminar
        process.wait()

        time.sleep(30)

with open('saida_sequencial16.txt', 'w') as output_file:
    output_file.write("Distancia sequencial paralelo total\n")
    # Itere 20 vezes
    for i in range(20):
        # Execute o programa C
        process = subprocess.Popen("./tsp", stdin=open("entrada16", "r"), stdout=subprocess.PIPE, text=True)
        
        # Obtenha a saída
        output, _ = process.communicate()
        
        # Escreva a saída no arquivo
        output_file.write(output)
        print("execução " + str(i) + " escrita no arquivo")
        # Aguarde até o processo terminar
        process.wait()

        time.sleep(30)

with open('saida_sequencial17.txt', 'w') as output_file:
    output_file.write("Distancia sequencial paralelo total\n")
    # Itere 20 vezes
    for i in range(20):
        # Execute o programa C
        process = subprocess.Popen("./tsp", stdin=open("entrada17", "r"), stdout=subprocess.PIPE, text=True)
        
        # Obtenha a saída
        output, _ = process.communicate()
        
        # Escreva a saída no arquivo
        output_file.write(output)
        print("execução " + str(i) + " escrita no arquivo")
        # Aguarde até o processo terminar
        process.wait()

        time.sleep(30)
