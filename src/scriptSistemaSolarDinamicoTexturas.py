import xml.etree.ElementTree as ET
import xml.dom.minidom
import random

#* Gerar planetas

#* Unit: Thousand Miles

# SOL
solDiametro = 432.450
solTexture = "sunmap.jpg"

# Mercurio
mercurioDistancia = 36000
mercurioDiametro = 3.031
mercurioAngulo = 11
mercurioTexture = "mercurymap.jpg"

# Venus
venusDistancia = 67100
venusDiametro = 7.521
venusAngulo = 28
venusTexture = "venusmap.jpg"

# Terra
terraDistancia = 92900
terraDiametro = 7.926
terraAngulo = 45
terraTexture = "earthmap.jpg"

# Lua
luaDistancia = 1200000
luaDiametro = 2.1
luaAngulo = 20
luaTexture = "moonmap.jpg"

# Marte
marteDistancia = 141500
marteDiametro = 4.221
marteAngulo = 85
marteTexture = "marsmap.jpg"

# Jupiter
jupiterDistancia = 483400
jupiterDiametro = 88.734
jupiterAngulo = 541
jupiterTexture = "jupitermap.jpg"

# Saturno
saturnoDistancia = 886700
saturnoDiametro = 74.566
saturnoAngulo = 1344
saturnoTexture = "saturnmap.jpg"

# Urano
uranoDistancia = 1782700
uranoDiametro = 31.566
uranoAngulo = 3835
uranoTexture = "uranusmap.jpg"

# Neptuno
neptunoDistancia = 2794300
neptunoDiametro = 30.199
neptunoAngulo = 7523
neptunoTexture = "neptunemap.jpg"

# Plutao
plutaoDistancia = 3666100
plutaoDiametro = 1.450
plutaoAngulo = 11320
plutaoTexture = "plutomap.jpg"

# Cintura Asteroides
cinturaAsteroidesDistanciaMin = 204430
cinturaAsteroidesDistanciaMax = 297450
cinturaAsteroidesNumAsteroides = 1000     # 1000
cinturaAsteroidesDiametro = 0.005

# Cintura de Kuiper
cinturaKuiperDistanciaMin = 2790000
cinturaKuiperDistanciaMax = 4650000
cinturaKuiperNumAsteroides = 1000     # 10000
cinturaKuiperDiametro = 0.015


planetas = [
        ("Mercurio", mercurioDistancia, mercurioDiametro, mercurioAngulo, mercurioTexture),
        ("Venus", venusDistancia, venusDiametro, venusAngulo, venusTexture),
        ("Terra", terraDistancia, terraDiametro, terraAngulo, terraTexture),
        ("Marte", marteDistancia, marteDiametro, marteAngulo, marteTexture),
        ("Jupiter", jupiterDistancia, jupiterDiametro, jupiterAngulo, jupiterTexture),
        ("Saturno", saturnoDistancia, saturnoDiametro, saturnoAngulo, saturnoTexture),
        ("Urano", uranoDistancia, uranoDiametro, uranoAngulo, uranoTexture),
        ("Neptuno", neptunoDistancia, neptunoDiametro, neptunoAngulo, neptunoTexture),
        ("Plutao", plutaoDistancia, plutaoDiametro, plutaoAngulo, plutaoTexture),
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
far = 1000000000

escalaDistancia = 0.002
escalaPlanetas = 0.5
escalaAsteroides = 50
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

def criaPlaneta (nome, distancia, diametro, angulo, texture, grupo):

    transform = criaElemento("transform", pai = grupo)
    
    # Rotação com tempo para fazer o planeta girar à volta do sol
    criaElemento("rotate", {"time": angulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
    
    # Depois o planeta é transladado para a distância correta
    criaElemento("translate", {"x": (solDiametro * escalaSol) + (distancia * escalaDistancia), "y": 0, "z": 0}, transform)
    
    # Roda o próprio planeta no eixo (ex: rotação própria)
    criaElemento("rotate", {"time": 5.0, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)

    # Escalado para o tamanho certo
    criaElemento("scale", {"x": diametro * escalaPlanetas, "y": diametro * escalaPlanetas, "z": diametro * escalaPlanetas}, transform)

    models = criaElemento("models", pai=grupo)
    model = criaElemento("model", {"file": f"planet.3d"}, models)
    criaElemento("texture", {"file": texture}, model)

    if nome == "Saturno" or nome == "Urano":
        subgrupo = criaElemento("group", pai=grupo)
        transform = criaElemento("transform", pai=subgrupo)
        angulo = 20
        alturaDonut = 0.1
        criaElemento("rotate", {"angle": angulo, "x": eixoRodarAsteroidesX, "y": eixoRodarAsteroidesY, "z": eixoRodarAsteroidesZ}, transform)
        criaElemento("scale", {"x": 1, "y": alturaDonut, "z": 1}, transform)

        models = criaElemento("models", pai=subgrupo)
        criaElemento("model", {"file": f"anelsaturno.3d"}, models)

    if nome == "Terra":
        subgrupo = criaElemento("group", pai=grupo)             # lua
        transform = criaElemento("transform", pai=subgrupo)

        criaElemento("rotate", {"time": luaAngulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
    
        # Depois o planeta é transladado para a distância correta
        criaElemento("translate", {"x": (luaDistancia / distancia), "y": 0, "z": 0}, transform)

        # Roda o próprio planeta no eixo (ex: rotação própria)
        criaElemento("rotate", {"time": 1.0, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)

        # Escalado para o tamanho certo
        criaElemento("scale", {"x": luaDiametro / diametro , "y": luaDiametro / diametro , "z": luaDiametro / diametro }, transform)

        models = criaElemento("models", pai=subgrupo)
        model = criaElemento("model", {"file": f"planet.3d"}, models)
        criaElemento("texture", {"file": luaTexture}, model)



def geradorXml():

    # Cria o elemento world
    world = criaElemento("world")

    # Cria o elemento window
    criaElemento("window", {"width": 512, "height": 512}, world)

    # Cria o elemento camera
    camera = criaElemento("camera", pai = world)

    # Definições da camera
    criaElemento("position", {"x": positionX, "y": positionY, "z": positionZ}, camera)
    criaElemento("lookAt", {"x": lookAtX, "y": lookAtY, "z": lookAtZ}, camera)
    criaElemento("up", {"x": upX, "y": upY, "z": upZ}, camera)
    criaElemento("projection", {"fov": fov, "near": near, "far": far}, camera)

    # Luz do sol
    lights = criaElemento("lights", pai = world)
    criaElemento("light", {"type": "point", "posx": 0, "posy": 0, "posz": 0}, lights)

    # SOL
    grupo = criaElemento("group", pai = world)
    transform = criaElemento("transform", pai = grupo)
    criaElemento("scale", {"x": solDiametro * escalaPlanetas * escalaSol, "y": solDiametro * escalaPlanetas * escalaSol, "z": solDiametro * escalaPlanetas * escalaSol}, transform)

    models = criaElemento("models", pai = grupo)
    model = criaElemento("model", {"file": f"planet.3d"}, models)
    criaElemento("texture", {"file": solTexture}, model);
    color = criaElemento("color", pai = model)
    criaElemento ("emissive", {"R": 255, "G": 255, "B": 255}, color)

    

    for (nome, distancia, diametro, angulo, texture) in planetas:
        grupo = criaElemento("group", pai = world)
        criaPlaneta (nome, distancia, diametro, angulo, texture, grupo)
        print (f"{nome} foi criado com sucesso.")

    # Cintura de Asteroides
    grupoAsteroides = criaElemento("group", pai = world)
    i = 0
    while (i < cinturaAsteroidesNumAsteroides):

        angulo = numero_aleatorio (0, 359)
        angulo2 = ((numero_aleatorio (0, 100) / 100) * 5) - 2.5
        distancia = numero_aleatorio (cinturaAsteroidesDistanciaMin, cinturaAsteroidesDistanciaMax)
        diametro = cinturaAsteroidesDiametro * (numero_aleatorio(20, 200) / 100) # variacao 20 a 200% tamanho

        grupo = criaElemento("group", pai = grupoAsteroides)
        transform = criaElemento("transform", pai = grupo)
        criaElemento("rotate", {"angle": angulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
        criaElemento("rotate", {"angle": angulo2, "x": eixoRodarAsteroidesX, "y": eixoRodarAsteroidesY, "z": eixoRodarAsteroidesZ}, transform)
        criaElemento("translate", {"x": (solDiametro * escalaSol) + (distancia * escalaDistancia), "y": 0, "z": 0}, transform)
        criaElemento("scale", {"x": diametro * escalaAsteroides, "y": diametro * escalaAsteroides, "z": diametro * escalaAsteroides}, transform)

        models = criaElemento("models", pai = grupo)
        model = criaElemento("model", {"file": f"asteroide.3d"}, models)
        criaElemento("texture", {"file": "asteroid.jpg"}, model);
        i += 1

    # Cintura de Asteroides 2
    grupoAsteroides = criaElemento("group", pai = world)
    i = 0
    while (i < cinturaAsteroidesNumAsteroides):

        angulo = numero_aleatorio (0, 359)
        angulo2 = ((numero_aleatorio (0, 100) / 100) * 5) - 2.5
        distancia = numero_aleatorio (cinturaKuiperDistanciaMin, cinturaKuiperDistanciaMax)
        diametro = cinturaKuiperDiametro * (numero_aleatorio(20, 200) / 100) # variacao 20 a 200% tamanho

        grupo = criaElemento("group", pai = grupoAsteroides)
        transform = criaElemento("transform", pai = grupo)
        criaElemento("rotate", {"angle": angulo, "x": eixoRodarPlanetasX, "y": eixoRodarPlanetasY, "z": eixoRodarPlanetasZ}, transform)
        criaElemento("rotate", {"angle": angulo2, "x": eixoRodarAsteroidesX, "y": eixoRodarAsteroidesY, "z": eixoRodarAsteroidesZ}, transform)
        criaElemento("translate", {"x": (solDiametro * escalaSol) + (distancia * escalaDistancia), "y": 0, "z": 0}, transform)
        criaElemento("scale", {"x": diametro * escalaAsteroides, "y": diametro * escalaAsteroides, "z": diametro * escalaAsteroides}, transform)

        models = criaElemento("models", pai = grupo)
        model = criaElemento("model", {"file": f"asteroide.3d"}, models)
        criaElemento("texture", {"file": "asteroid.jpg"}, model);
        i += 1

    # Cometa
    grupoCometa = criaElemento("group", pai=world)
    transform = criaElemento("transform", pai=grupoCometa)
    translate = criaElemento("translate", {"time": "20", "align": "true"}, transform)

        # Pontos da trajetória (meio curva em 3D para parecer bonito)
    criaElemento("point", {"x": "-1000", "y": "1000", "z": "0"}, translate)
    criaElemento("point", {"x": "0", "y": "200", "z": "300"}, translate)
    criaElemento("point", {"x": "500", "y": "-500", "z": "0"}, translate)
    criaElemento("point", {"x": "1000", "y": "0", "z": "-500"}, translate)

    criaElemento("rotate", {"angle": "-75", "x": "1", "y": "0", "z": "0"}, transform)
    criaElemento("scale", {"x": "4.0", "y": "4.0", "z": "4.0"}, transform)

    models = criaElemento("models", pai=grupoCometa)
    model = criaElemento("model", {"file": "bezier_10.3d"}, models)
    criaElemento("texture", {"file": "teapot.jpg"}, model);



    return world

def generate_xml():
    world = geradorXml()
    rough_string = ET.tostring(world, encoding="utf-8")
    reparsed = xml.dom.minidom.parseString(rough_string)

    return reparsed.toprettyxml(indent="  ")

def main():
    xml_string = generate_xml()
    with open("test files/planets3.xml", "w", encoding="utf-8") as f:
        f.write(xml_string)
    print("XML gerado com sucesso: planets3.xml")

if __name__ == "__main__":
    main()