import os
import numpy as np
from tqdm import tqdm
from shapely.geometry import Polygon
import time

def GetFileFromThisRootDir(dir, ext=None):
    allfiles = []
    needExtFilter = (ext != None)
    for root, dirs, files in os.walk(dir):
        for filespath in files:
            filepath = os.path.join(root, filespath)
            extension = os.path.splitext(filepath)[1][1:]
            if needExtFilter and extension in ext:
                allfiles.append(filepath)
            elif not needExtFilter:
                allfiles.append(filepath)
    return allfiles

def CombImage(srcpath, dstpath):
    imageNameList = [s for s in srcpath if 'png' not in s]
    IoUThreshold = 0.31
    for imageName in imageNameList:
        imgelist = GetFileFromThisRootDir(srcpath)
        print(len(imgelist["normal"]))
        modeList = ["row", "column"]
        for mode in modeList:
            for i in tqdm(imgelist[mode], desc=imageName + "." + mode):
                a = []
                for k in range(4):
                    point = i["location"]["points"][k]
                    a.append(point["x"])
                    a.append(point["y"])
                quadrilateralA = Polygon(np.array(a).reshape(4, 2)).convex_hull
                identified = False
                for j in imgelist["normal"]:
                    b = []
                    for k in range(4):
                        point = j["location"]["points"][k]
                        b.append(point["x"])
                        b.append(point["y"])
                    quadrilateralB = Polygon(np.array(b).reshape(4, 2)).convex_hull
                    intersectionArea = quadrilateralA.intersection(quadrilateralB).area  # 相交面积
                    unionArea = quadrilateralA.area + quadrilateralB.area - intersectionArea
                    IoU = intersectionArea / unionArea
                    if IoU > IoUThreshold:
                        identified = True
                        break
                if identified == False:
                    imgelist["normal"].append(i)
            print(len(imgelist["normal"]))
        duplicateAnchorList = []
        for i in tqdm(range(len(imgelist["normal"]) - 1), desc=imageName + ".duplicate removal"):
            a = []
            for k in range(4):
                point = imgelist["normal"][i]["location"]["points"][k]
                a.append(point["x"])
                a.append(point["y"])
            quadrilateralA = Polygon(np.array(a).reshape(4, 2)).convex_hull
            duplicate = False
            for j in range(i + 1, len(imgelist["normal"])):
                b = []
                for k in range(4):
                    point = imgelist["normal"][j]["location"]["points"][k]
                    b.append(point["x"])
                    b.append(point["y"])
                quadrilateralB = Polygon(np.array(b).reshape(4, 2)).convex_hull
                intersectionArea = quadrilateralA.intersection(quadrilateralB).area  # 相交面积
                unionArea = quadrilateralA.area + quadrilateralB.area - intersectionArea
                IoU = intersectionArea / unionArea
                if IoU > IoUThreshold:
                    duplicate = True
                    break
            if duplicate == True:
                duplicateAnchorList.append(i)
        for i in range(len(duplicateAnchorList)):
            imgelist["normal"].pop(duplicateAnchorList[len(duplicateAnchorList) - 1 - i])
        print(len(imgelist["normal"]))



if __name__ == '__main__':
    start_time = time.time()

    CombImage(r'../data/test/labels', r'../data/test/labelTxt')

    end_time = time.time()
    print("DONE")

