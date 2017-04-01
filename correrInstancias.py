import subprocess
from timeit import timeit


if __name__ == '__main__':
	TIMEOUT = 30

	instancias = []
	with open("InstanciasACorrer.txt","r") as f:
		lines = f.readlines()
		for line in lines:
			line = line.replace("\n","").replace("\r","")
			line = line.split("\t")
			instancias += [[line[0],int(line[1])]]

	rango = input("Rango a correr: (inclusivo, separado por -): ")
	rango = rango.split("-")

	if len(rango) == 1:
		rangoNum = range(int(rango[0])-1,int(rango[0]))
	else:
		rangoNum = range(int(rango[0])-1,int(rango[1]))

	print("Corriendo instancias...")
	print("instancia","Vo","%dHeur","t(seg)")

	for i in rangoNum:
		solOpt = int(instancias[i][1])
		salida = ""
		path = "instanciasPRPP/" + instancias[i][0]

		try:
			resultado = subprocess.check_output("./branchAndBound .\\" + path,timeout=30)
		except:
			print("TIMED OUT")
			continue
		salida = resultado.decode('utf-8')
		salida = salida.split("\n")
		solucionHeur = int(salida[0].split("\t")[1])
		solucionBranchAndBound = int(solOpt)
		if len(salida) > 1:
			solucionBranchAndBound = int(salida[1].split("\t")[1])
			time = float(salida[2].split("\t")[1])

		if solucionBranchAndBound != solOpt:
			print("PANIC",solucionBranchAndBound,solOpt)

		desvHeur = 0
		if (solOpt != 0):
			desvHeur = 100*((solOpt - solucionHeur)/solOpt)
		print(path + "\t" + str(solOpt) + "\t" \
				 + str(solucionHeur) + "\t"
				 + str.format("{0:.03f}",desvHeur) + "\t" \
				 + str.format("{0:.03f}",time))