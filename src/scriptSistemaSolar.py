import xml.etree.ElementTree as ET
import xml.dom.minidom
import random

#* Gerar planetas

#* Unit: Thousand Miles

# SOL
solDiametro = 432.450

# Mercurio
mercurioDistancia = 36000
mercurioDiametro = 3.031
mercurioAngulo = 30

# Venus
venusDistancia = 67100
venusDiametro = 7.521
venusAngulo = 60

# Terra
terraDistancia = 92900
terraDiametro = 7.926
terraAngulo = 10

# Marte
marteDistancia = 141500
marteDiametro = 4.221
marteAngulo = 120

# Jupiter
jupiterDistancia = 483400
jupiterDiametro = 88.734
jupiterAngulo = 270

# Saturno
saturnoDistancia = 886700
saturnoDiametro = 74.566
saturnoAngulo = 170

# Urano
uranoDistancia = 1782700
uranoDiametro = 31.566
uranoAngulo = 40

# Neptuno
neptunoDistancia = 2794300
neptunoDiametro = 30.199
neptunoAngulo = 15

# Plutao
plutaoDistancia = 3666100
plutaoDiametro = 1.450
plutaoAngulo = 330

# Cinto Asteroides
cintoAsteroidesDistanciaMin = 204430
cintoAsteroidesDistanciaMax = 297450
cintoAsteroidesNumAsteroides = 2000
cintoAsteroidesDiametro = 0.005


planetas = [
        ("Mercurio", mercurioDistancia, mercurioDiametro, mercurioAngulo),
        ("Venus", venusDistancia, venusDiametro, venusAngulo),
        ("Terra", terraDistancia, terraDiametro, terraAngulo),
        ("Marte", marteDistancia, marteDiametro, marteAngulo),
        ("Jupiter", jupiterDistancia, jupiterDiametro, jupiterAngulo),
        ("Saturno", saturnoDistancia, saturnoDiametro, saturnoAngulo),
        ("Urano", uranoDistancia, uranoDiametro, uranoAngulo),
        ("Neptuno", neptunoDistancia, neptunoDiametro, neptunoAngulo),
        ("Plutao", plutaoDistancia, plutaoDiametro, plutaoAngulo),
    ]

# Eixo de rotação dos planetas (Vertical)
eixoRodarPlanetasX = 0
eixoRodarPlanetasY = 1
eixoRodarPlanetasZ = 0

eixoRodarAsteroidesX = 0
eixoRodarAsteroidesY = 0
eixoRodarAsteroidesZ = 1

#* Definições de camera

# Posição
positionX = 300
positionY = 300
positionZ = 300

# Olha para
lookAtX = 0
lookAtY = 0
lookAtZ = 0

# Vetor CimaupX = 0
upX = 0
upY = 1
upZ = 0

# Projeção
fov = 60
near = 1
far = 1000000

escalaDistancia = 0.001
escalaPlanetas = 1
escalaAsteroides = 30
escalaSol = 0.60

def numero_aleatorio(minimo: int, maximo: int) -> int:
    return random.randint(minimo, maximo)


def criaElemento(tag, attributos=None, pai=None):

    elemento = ET.Element(tag)

    if attributos:
        elemento.attrib = {k: str(v) for k, v in attributos.items()}

    if pai is not None:
        pai.append(elemento)

    return elemento

def criaPlaneta (distancia, diametro, angulo, grupo):

    transform = criaElemento("transform", pai = grupo)
    criaElemento("rotate", {"angle": angulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
    criaElemento("translate", {"x": (solDiametro * escalaSol) + (distancia * escalaDistancia), "y": 0, "z": 0}, transform)
    criaElemento("scale", {"x": diametro * escalaPlanetas, "y": diametro * escalaPlanetas, "z": diametro * escalaPlanetas}, transform)

    models = criaElemento("models", pai = grupo)
    criaElemento("model", {"file": f"planet.3d"}, models)

def geradorXml():

    # Cria o elemento world
    world = criaElemento("world")

    # Cria o elemento window
    window = criaElemento("window", {"width": 512, "height": 512}, world)

    # Cria o elemento camera
    camera = criaElemento("camera", pai = world)

    # Definições da camera
    criaElemento("position", {"x": positionX, "y": positionY, "z": positionZ}, camera)
    criaElemento("lookAt", {"x": lookAtX, "y": lookAtY, "z": lookAtZ}, camera)
    criaElemento("up", {"x": upX, "y": upY, "z": upZ}, camera)
    criaElemento("projection", {"fov": fov, "near": near, "far": far}, camera)

    # SOL
    grupo = criaElemento("group", pai = world)
    transform = criaElemento("transform", pai = grupo)
    criaElemento("scale", {"x": solDiametro * escalaPlanetas * escalaSol, "y": solDiametro * escalaPlanetas * escalaSol, "z": solDiametro * escalaPlanetas * escalaSol}, transform)

    models = criaElemento("models", pai = grupo)
    criaElemento("model", {"file": f"planet.3d"}, models)
    

    for (nome, distancia, diametro, angulo) in planetas:
        grupo = criaElemento("group", pai = world)
        criaPlaneta (distancia, diametro, angulo, grupo)
        print (f"{nome} foi criado com sucesso.")

    # Cinto de Asteroides
    grupoAsteroides = criaElemento("group", pai = world)
    i = 0
    while (i < cintoAsteroidesNumAsteroides):

        angulo = numero_aleatorio (0, 359)
        angulo2 = ((numero_aleatorio (0, 100) / 100) * 5) - 2.5
        distancia = numero_aleatorio (cintoAsteroidesDistanciaMin, cintoAsteroidesDistanciaMax)
        diametro = cintoAsteroidesDiametro * (numero_aleatorio(20, 200) / 100) # variacao 20 a 200% tamanho

        grupo = criaElemento("group", pai = grupoAsteroides)
        transform = criaElemento("transform", pai = grupo)
        criaElemento("rotate", {"angle": angulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
        criaElemento("rotate", {"angle": angulo2, "x": eixoRodarAsteroidesX, "y": eixoRodarAsteroidesY, "z": eixoRodarAsteroidesZ}, transform)
        criaElemento("translate", {"x": (solDiametro * escalaSol) + (distancia * escalaDistancia), "y": 0, "z": 0}, transform)
        criaElemento("scale", {"x": diametro * escalaAsteroides, "y": diametro * escalaAsteroides, "z": diametro * escalaAsteroides}, transform)

        models = criaElemento("models", pai = grupo)
        criaElemento("model", {"file": f"asteroide.3d"}, models)
        i += 1
    

    return world

def generate_xml():
    world = geradorXml()
    rough_string = ET.tostring(world, encoding="utf-8")
    reparsed = xml.dom.minidom.parseString(rough_string)

    return reparsed.toprettyxml(indent="  ")

def main():
    #tree = generate_xml()
    #tree.write("planets.xml", encoding="utf-8", xml_declaration=True)

    xml_string = generate_xml()
    with open("planets.xml", "w", encoding="utf-8") as f:
        f.write(xml_string)
    print("XML gerado com sucesso: planets.xml")

    print("XML gerado com sucesso: planets.xml")

if __name__ == "__main__":
    main()