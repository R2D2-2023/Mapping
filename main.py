import cv2
import math
import numpy as np
from win32api import GetSystemMetrics

class MouseData:
    def __init__(self):
        self.matrix = None
        self.cell_size = 0
        self.img = None
        self.origImg = None
        self.counter = 0
        self.checkpoint_counter = -5
        self.route = []
        self.meetPunten = []
        self.hoeken = []
        self.robotX = 10
        self.robotY = 10
        self.mode = 0

class GridData:
    def __init__(self):
        self.beginX = 0
        self.beginY = 0
        self.endX = 0
        self.endY = 0

class StoreData:
    def __init__(self):
        self.gData = GridData()
        self.MData = MouseData()

def update_grid_image(matrix, data):
    grid_color = (0, 0, 255)
    grid_color_green = (0, 255, 0)
    grid_color_checkpoint = (255, 0, 255)
    grid_color_corner = (255, 0, 0)
    grid_img = data.MData.origImg.copy()
    cell_size = data.MData.cell_size

    for i in range(len(matrix)):
        y = data.gData.beginY + i * cell_size
        for j in range(len(matrix[i])):
            x = data.gData.beginX + j * cell_size
            value = matrix[i][j]
            rect_color = None
            if value == 0:
                rect_color = grid_color
            elif value == 9999:
                rect_color = (100, 100, 100)
            elif value == 6000:
                rect_color = grid_color_corner
            else:
                if value < 0:
                    rect_color = grid_color_checkpoint
                else:
                    rect_color = grid_color_green
            cv2.rectangle(grid_img, (x, y), (x + cell_size, y + cell_size), rect_color, 1 if value == 0 else -1)
    
    cv2.imshow("Grid Image", grid_img)
    
def edit_image(filename):
    screenWidth = GetSystemMetrics(0)
    screenHeight = GetSystemMetrics(1)

    img = cv2.imread(filename, cv2.IMREAD_COLOR)
    if img is None:
        print("Could not read the image")

    imageAspectRatio = img.shape[1] / img.shape[0]
    
    maxWidth = screenWidth
    maxHeight = screenHeight
 
    scaledWidth = maxWidth
    scaledHeight = int(scaledWidth / imageAspectRatio)

    if scaledHeight > maxHeight:
        scaledHeight = maxHeight
        scaledWidth = int(scaledHeight * imageAspectRatio)

    posX = (screenWidth - scaledWidth) // 2
    posY = (screenHeight - scaledHeight) // 2

    cv2.namedWindow("Grid Image")
    cv2.moveWindow("Grid Image", posX, posY)
    img = cv2.resize(img, (scaledWidth, scaledHeight))

    return img

def get_roi(img):
    hsv_img = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # Define the lower and upper bounds of the black color range
    lower = np.array([0, 0, 0])
    upper = np.array([180, 255, 30])

    # Create a mask to filter out non-black pixels
    mask = cv2.inRange(hsv_img, lower, upper)
    
    # Apply the mask to the original image
    maskImg = cv2.bitwise_and(img, img, mask=mask)

    # Find the coordinates of black pixels
    coords = []
    for y in range(mask.shape[0]):
        for x in range(mask.shape[1]):
            if mask[y, x] == 255:
                coords.append((x, y))

    # Find the highest, lowest, leftmost, and rightmost points
    highestPoint = coords[0]
    lowestPoint = coords[0]
    leftmostPoint = coords[0]
    rightmostPoint = coords[0]

    for coord in coords:
        if coord[1] < highestPoint[1]:
            highestPoint = coord
        if coord[1] > lowestPoint[1]:
            lowestPoint = coord
        if coord[0] < leftmostPoint[0]:
            leftmostPoint = coord
        if coord[0] > rightmostPoint[0]:
            rightmostPoint = coord

    startPoint = (leftmostPoint[0], highestPoint[1])  # Starting coordinate of the grid
    endPoint = (rightmostPoint[0], lowestPoint[1])  # Ending coordinate of the grid
    
    return maskImg, startPoint, endPoint

def OnMouse(event, x, y, flags, user_data):
    data = user_data
    cell_size = data.MData.cell_size
    matrix = data.MData.matrix
    matrix_width = len(matrix[0])

    j = (x - data.gData.beginX) // cell_size
    i = (y - data.gData.beginY) // cell_size

    if event == cv2.EVENT_MOUSEMOVE:
        if i >= 0 and i < len(matrix) and j >= 0 and j < matrix_width:
            current_cell = matrix[i][j]  # Get the current value from the matrix
            if data.MData.mode == 0:
                if flags & cv2.EVENT_FLAG_LBUTTON:
                    if current_cell == 0 and current_cell != 1:
                        data.MData.counter += 1
                        current_cell = data.MData.counter
                        data.MData.route.append((i, j))
                        print(f"{j}/{i}: {data.MData.counter}")
                        print("route: ", end="")
                        route_ss = ""
                        for route_coord in data.MData.route:
                            route_ss += f"{route_coord[0]}/{route_coord[1]},"
                        print(route_ss)
                        update_grid_image(matrix, data)
                elif flags & cv2.EVENT_FLAG_RBUTTON:
                    if current_cell != 0:
                        is_in_route_or_meetpunten = False
                        route_it = next((index for index, coord in enumerate(data.MData.route) if coord == (i, j)), None)
                        meetpunten_it = next((index for index, coord in enumerate(data.MData.meetPunten) if coord == (i, j)), None)
                        if route_it is not None or meetpunten_it is not None:
                            is_in_route_or_meetpunten = True
                            if route_it is not None:
                                data.MData.route.pop(route_it)
                            if meetpunten_it is not None:
                                data.MData.meetPunten.pop(meetpunten_it)
                        if not is_in_route_or_meetpunten:
                            current_cell = 0
                            update_grid_image(matrix, data)
            elif data.MData.mode == 1:
                if flags & cv2.EVENT_FLAG_LBUTTON:
                    if current_cell == 0:
                        current_cell = 9999
                        data.MData.robotX = i
                        data.MData.robotY = j
                        update_grid_image(matrix, data)
                elif flags & cv2.EVENT_FLAG_RBUTTON:
                    if current_cell == 0:
                        data.MData.checkpoint_counter -= 1
                        current_cell = data.MData.checkpoint_counter
                        data.MData.meetPunten.append((i, j))
                        print("meetPunten: ", end="")
                        meetpunten_ss = ""
                        for meetpunten_coord in data.MData.meetPunten:
                            meetpunten_ss += f"{meetpunten_coord[0]}/{meetpunten_coord[1]},"
                        print(meetpunten_ss)
                        update_grid_image(matrix, data)
            elif data.MData.mode == 2:  # Add mode 2 logic here
                if flags & cv2.EVENT_FLAG_LBUTTON:
                    if current_cell == 0:
                        current_cell = 6000
                        data.MData.hoeken.append((i, j))
                        print("hoeken: ", end="")
                        hoeken_ss = ""
                        for hoeken_coord in data.MData.hoeken:
                            hoeken_ss += f"{hoeken_coord[0]}/{hoeken_coord[1]},"
                        print(hoeken_ss)
                        update_grid_image(matrix, data)
                elif flags & cv2.EVENT_FLAG_RBUTTON:
                        update_grid_image(matrix, data)
                
            matrix[i][j] = current_cell  # Update the value in the matrix

    elif event == cv2.EVENT_MOUSEWHEEL:
        if i >= 0 and i < len(matrix) and j >= 0 and j < matrix_width:
            data.MData.mode = (data.MData.mode + 1) % 3  # Cycle through modes 0, 1, and 2
            print(f"mode: {data.MData.mode}")

def setData(img, begin, end, origImg):
    longest_dim = max(img.shape[1], img.shape[0])
    cell_size = int(np.ceil(longest_dim / 300))

    uData = StoreData()
    uData.gData.beginX = begin[0]
    uData.gData.beginY = begin[1]
    uData.gData.endX = end[0]
    uData.gData.endY = end[1]

    roi = (begin[0], begin[1], end[0] - begin[0], end[1] - begin[1])
    roiImage = img[roi[1]:roi[1]+roi[3], roi[0]:roi[0]+roi[2]]

    width = int(np.ceil(roiImage.shape[1] / cell_size))
    height = int(np.ceil(roiImage.shape[0] / cell_size))

    matrix = [[0] * width for _ in range(height)]

    uData.MData.matrix = matrix
    uData.MData.cell_size = cell_size
    uData.MData.img = img
    uData.MData.origImg = origImg
    
    return matrix, uData

def write_to_txt(uData, filename):
    # Convert tuples to strings
    route_str = [f"{coord[0]}/{coord[1]}" for coord in uData.MData.route]
    meetPunten_str = [f"{coord[0]}/{coord[1]}" for coord in uData.MData.meetPunten]
    hoeken_str = [f"{coord[0]}/{coord[1]}" for coord in uData.MData.hoeken]

    # Combine the arrays into a single string
    data = ",".join(route_str) + "\n" + ",".join(meetPunten_str) + "\n" + ",".join(hoeken_str)
    
    with open(filename, "w") as file:
        file.write(data)


def create_grid_image(filename):
    
    screenWidth = GetSystemMetrics(0)
    screenHeight = GetSystemMetrics(1)

    origImg = cv2.imread(filename, cv2.IMREAD_COLOR)
    if origImg is None:
        print("Could not read the image")

    imageAspectRatio = origImg.shape[1] / origImg.shape[0]
    
    maxWidth = screenWidth
    maxHeight = screenHeight
 
    scaledWidth = maxWidth
    scaledHeight = int(scaledWidth / imageAspectRatio)

    if scaledHeight > maxHeight:
        scaledHeight = maxHeight
        scaledWidth = int(scaledHeight * imageAspectRatio)

    posX = (screenWidth - scaledWidth) // 2
    posY = (screenHeight - scaledHeight) // 2

    cv2.namedWindow("Grid Image")
    cv2.moveWindow("Grid Image", posX, posY)
    origImg = cv2.resize(origImg, (scaledWidth, scaledHeight))
    
    
    img, begin, end = get_roi(origImg)
    

    matrix, uData = setData(img, begin, end, origImg)
    

    cv2.setMouseCallback("Grid Image", OnMouse, uData)
    update_grid_image(matrix, uData)

    cv2.waitKey(0)
    
    write_to_txt(uData, "arrays.txt")

    return matrix


def main():
    
    matrix = create_grid_image("Toerk_map.png")


if __name__ == '__main__':
    main()