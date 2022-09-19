from multiprocessing import Pool

from tqdm import tqdm
import json
import numpy
from shapely.geometry import Polygon
import os
import cv2

# 定义参数
imageSetName = "2512res_corner"
imageFilePath = "../data/Cropped Data/" + imageSetName + "/"

def integrate(imageName):
    IoUThreshold = 0.3
        
    resultJson = json.load(open(imageFilePath + imageName + "/result_2.json"))
    modeList = ["row", "column"]
    for mode in modeList:
        for i in tqdm(resultJson[mode], desc=imageName + "." + mode):
            a = []
            for k in range(4):
                point = i["location"]["points"][k]
                a.append(point["x"])
                a.append(point["y"])
            quadrilateralA = Polygon(numpy.array(a).reshape(4, 2)).convex_hull
            identified = False
            for j in resultJson["normal"]:
                b = []
                for k in range(4):
                    point = j["location"]["points"][k]
                    b.append(point["x"])
                    b.append(point["y"])
                quadrilateralB = Polygon(numpy.array(b).reshape(4, 2)).convex_hull
                intersectionArea = quadrilateralA.intersection(quadrilateralB).area  # 相交面积
                unionArea = quadrilateralA.area + quadrilateralB.area - intersectionArea
                IoU = intersectionArea / unionArea
                if IoU > IoUThreshold:
                    identified = True
                    break
            if identified == False:
                resultJson["normal"].append(i)
    duplicateAnchorList = []
    for i in tqdm(range(len(resultJson["normal"]) - 1), desc=imageName + ".duplicate removal"):
        a = []
        for k in range(4):
            point = resultJson["normal"][i]["location"]["points"][k]
            a.append(point["x"])
            a.append(point["y"])
        quadrilateralA = Polygon(numpy.array(a).reshape(4, 2)).convex_hull
        duplicate = False
        for j in range(i + 1, len(resultJson["normal"])):
            b = []
            for k in range(4):
                point = resultJson["normal"][j]["location"]["points"][k]
                b.append(point["x"])
                b.append(point["y"])
            quadrilateralB = Polygon(numpy.array(b).reshape(4, 2)).convex_hull
            intersectionArea = quadrilateralA.intersection(quadrilateralB).area
            unionArea = quadrilateralA.area + quadrilateralB.area - intersectionArea
            IoU = intersectionArea / unionArea
            if IoU > IoUThreshold:
                duplicate = True
                break
        if duplicate == True:
            duplicateAnchorList.append(i)
    for i in range(len(duplicateAnchorList)):
        resultJson["normal"].pop(duplicateAnchorList[len(duplicateAnchorList) - 1 - i])
    if not os.path.exists("../temp/" + imageSetName + "/"):
        os.makedirs("../temp/" + imageSetName + "/")
    json.dump(resultJson, open("../temp/" + imageSetName + "/" + imageName + "_2.json", 'w'))


def draw_anchor_box(imageNameList):
    for imageName in imageNameList:
        imagePath = "../data/" + imageSetName + "/" + imageName + ".png"
        image = cv2.imread(imagePath)
        resultJson = json.load(open("../temp/" + imageSetName + "/" + imageName + "_2.json"))
        print(imageName, len(resultJson["normal"]))
        for anchorBox in resultJson["normal"]:
            a = []
            for k in range(4):
                point = anchorBox["location"]["points"][k]
                a.append((point["x"], point["y"]))
            points = numpy.array(a, numpy.int32).reshape(4, 2).reshape((-1, 1, 2))
            cv2.polylines(image, [points], True, (255, 0, 0))
        cv2.imwrite("../temp/" + imageSetName + "/" + imageName + "_2.png", image)


if __name__ == '__main__':
    imageNameList = os.listdir(imageFilePath)
    imageNameList = [s for s in imageNameList if 'png' not in s]

    with Pool(8) as p:
        p.map(integrate, imageNameList)
        
    draw_anchor_box(imageNameList)