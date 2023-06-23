import cv2
import math
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
            else:
                if value < 0:
                    rect_color = grid_color_checkpoint
                else:
                    rect_color = grid_color_green
            cv2.rectangle(grid_img, (x, y), (x + cell_size, y + cell_size), rect_color, 1 if value == 0 else -1)

    cv2.imshow("Grid Image", grid_img)



def check_surroundings(data):
    next_found = False
    for y in range(data.MData.robotY, data.MData.robotY + 1):
        for x in range(data.MData.robotX, data.MData.robotX + 1):
            if x < 0 or x >= 65 or y < 0 or y >= 232:
                continue
            if len(data.MData.route) <= data.MData.counter:
                return 0
            elif x == data.MData.route[data.MData.counter][0] and y == data.MData.route[data.MData.counter][1]:
                value = data.MData.matrix[x][y]
                data.MData.counter += 1
                print("COORDINATE FOUND")

                if value == -1:
                    # Coördinaat is een obstakel
                    return -1
                elif value == -2:
                    # Coördinaat is een checkpoint
                    return -2
                else:
                    # Coördinaat is een geldige volgende stap
                    return 1
            else:
                next_found = True

    if not next_found:
        # Er zijn geen volgende coördinaten in de route
        return 0
    
def edit_image():
    screenWidth = GetSystemMetrics(0)
    screenHeight = GetSystemMetrics(1)

    img = cv2.imread("Toerk_map.png", cv2.IMREAD_COLOR)
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

def OnMouse(event, x, y, flags, user_data):
    data = user_data
    cell_size = data.MData.cell_size
    matrix = data.MData.matrix
    matrix_width = len(matrix[0])

    j = (x - data.gData.beginX) // cell_size
    i = (y - data.gData.beginY) // cell_size

    if event == cv2.EVENT_MOUSEMOVE:
        if 0 <= i < len(matrix) and 0 <= j < matrix_width:
            current_cell = matrix[i][j]
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
                        route_it = None
                        meetpunten_it = None
                        for index, coord in enumerate(data.MData.route):
                            if coord == (i, j):
                                route_it = index
                                break
                        for index, coord in enumerate(data.MData.meetPunten):
                            if coord == (i, j):
                                meetpunten_it = index
                                break
                        if route_it is not None or meetpunten_it is not None:
                            is_in_route_or_meetpunten = True
                            if route_it is not None:
                                del data.MData.route[route_it]
                            if meetpunten_it is not None:
                                del data.MData.meetPunten[meetpunten_it]
                        if not is_in_route_or_meetpunten:
                            current_cell = 0
                            update_grid_image(matrix, data)



def create_grid_image(img, orig_img, begin, end):
    cv2.namedWindow("Grid Image")

    longest_dim = max(img.shape[1], img.shape[0])
    cell_size = math.ceil(longest_dim / 300)

    store_data = StoreData()
    store_data.gData.beginX = begin[0]
    store_data.gData.beginY = begin[1]
    store_data.gData.endX = end[0]
    store_data.gData.endY = end[1]

    roi = (begin[0], begin[1], end[0] - begin[0], end[1] - begin[1])
    roi_image = img[roi[1]:roi[1]+roi[3], roi[0]:roi[0]+roi[2]]

    width = math.ceil(roi_image.shape[1] / cell_size)
    height = math.ceil(roi_image.shape[0] / cell_size)

    matrix = [[0] * height for _ in range(width)]
    store_data.MData.matrix = matrix
    store_data.MData.cell_size = cell_size
    store_data.MData.img = roi_image
    store_data.MData.origImg = orig_img

    cv2.setMouseCallback("Grid Image", OnMouse, store_data)
    update_grid_image(matrix, store_data)

    while True:
        key = cv2.waitKey(1) & 0xFF

        if key == ord('q'):
            break

        elif key == ord('s'):
            # Start de robotbeweging
            while True:
                result = check_surroundings(store_data)
                if result == 0:
                    break
                elif result == -1:
                    # Obstakel gevonden, stop de beweging
                    print("Obstakel gevonden!")
                    break
                elif result == -2:
                    # Checkpoint bereikt
                    print("Checkpoint bereikt!")

        elif key == ord('r'):
            # Reset de robotpositie en tellers
            store_data.MData.robotX = 10
            store_data.MData.robotY = 10
            store_data.MData.counter = 0
            print("Robot positie gereset!")

    cv2.destroyAllWindows()


def main():
    img = cv2.imread("Toerk_map.png", cv2.IMREAD_COLOR)
    if img is None:
        print("Could not read the image")
        return

    screenWidth = GetSystemMetrics(0)
    screenHeight = GetSystemMetrics(1)
    begin = (20, 20)
    end = (screenWidth - 20, screenHeight - 20)

    orig_img = edit_image()

    create_grid_image(img, orig_img, begin, end)

if __name__ == '__main__':
    main()
