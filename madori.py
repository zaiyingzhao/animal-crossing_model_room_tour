import cv2
import numpy as np
import copy
from PIL import Image
import csv

# from IPython.display import display
drawing_image = cv2.imread("myhouse.jpeg")
drawing = False
ix = 0
iy = 0
roomsize = []
zahyo = []
points = []

# drawing_image = cv2.cvtColor(drawing_image, cv2.COLOR_BGR2GRAY)

def distance(ix, iy, x, y):
    return ((ix - x) ** 2 + (iy - y) ** 2) ** 0.5


# Adding Function Attached To Mouse Callback
def draw(event, x, y, flags, params):
    global ix, iy, drawing
    # Left Mouse Button Down Pressed
    if event == cv2.EVENT_LBUTTONDOWN:
        drawing = True
        ix = x
        iy = y
    if event == cv2.EVENT_LBUTTONUP:
        if drawing:
            # For Drawing Line
            cv2.line(
                drawing_image, pt1=(ix, iy), pt2=(x, y), color=(255, 0, 0), thickness=8
            )
            # print(distance(ix, iy, x, y))
            roomsize.append(distance(ix, iy, x, y))
            zahyo.append(((ix + x) / 2, (iy + y) / 2))
            points.append([ix,iy])
            points.append([x,y])

            ix = x
            iy = y
            # For Drawing Rectangle
            # cv2.rectangle(image,pt1=(ix,iy),pt2=(x,y),color=(255,255,255),thickness=3)
    if event == 4:
        drawing = False


# Making Window For The Image
cv2.namedWindow("Window")

# Adding Mouse CallBack Event
cv2.setMouseCallback("Window", draw)

# Starting The Loop So Image Can Be Shown
while True:
    cv2.imshow("Window", drawing_image)
    if cv2.waitKey(20) & 0xFF == ord("q"):
        # cv2.imwrite("samplepic_mod.jpg", drawing_image)
        break

cv2.destroyAllWindows()

# hsv = cv2.cvtColor(drawing_image, cv2.COLOR_BGR2HSV)

bgrLower = np.array([200, 0, 0])
bgrUpper = np.array([255, 10, 10])
img_mask = cv2.inRange(drawing_image, bgrLower, bgrUpper)
contour = cv2.bitwise_and(drawing_image, drawing_image, mask=img_mask)
cv2.imwrite("img_mask.jpg", img_mask)
cv2.imwrite("contour.jpg", contour)

# im_gray = cv2.cvtColor(contour, cv2.COLOR_BGR2GRAY)
# retval, im_bw = cv2.threshold(img_mask, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
# cv2.imwrite("test.jpg", im_gray)

# 輪郭の検出
contours, hierarchy = cv2.findContours(img_mask, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
print(len(contours), "contours found.")

for i in range(len(contours)):
    im_con = drawing_image.copy()
    # print("ID", i, "Area", cv2.contourArea(contours[i]))
    im_con = cv2.drawContours(im_con, contours, i, (0, 255, 0), 2)
    rinkaku = cv2.drawContours(contour, contours, 1, (0, 255, 0), 2)
    cv2.imwrite("rinkaku.jpg", rinkaku)
    cv2.imwrite("result" + str(i) + ".jpg", im_con)

# var = input("Please choose appropriate contours.\n")
var = 1
surface = cv2.contourArea(contours[int(var)])
print("Surface area: ", cv2.contourArea(contours[int(var)]))
var2 = input("Please input your room size. (tatami-mats)\n")
coefficient = (16200 * float(var2) / surface) ** 0.5
roomsize = [n * coefficient for n in roomsize]

# 輪郭塗りつぶし
room_image = copy.deepcopy(drawing_image)
cnt = contours[int(var)]
room_image = cv2.drawContours(room_image, [cnt], 0, (255, 0, 0), -1)
cv2.imwrite("painted_image.jpg", room_image)

im = np.array(Image.open("painted_image.jpg"))
lower = np.array([200, 0, 0])
upper = np.array([255, 150, 150])
im_mask = cv2.inRange(im, lower, upper)
cv2.imwrite("painted_image.jpg", im_mask)

for i in range(len(roomsize)):
    cv2.putText(
        drawing_image,
        str(int(roomsize[i])),
        (int(zahyo[i][0]), int(zahyo[i][1])),
        fontFace=cv2.FONT_HERSHEY_COMPLEX,
        fontScale=1,
        color=(255, 255, 0),
    )

cv2.imwrite("measured_room.jpg", drawing_image)

# 部屋を含む最小の長方形を取得
x_max = 0
y_max = 0
x_min = 500
y_min = 500
for i in range(len(points)):
    if points[i][0] < x_min: x_min = points[i][0]
    if points[i][0] > x_max: x_max = points[i][0]
    if points[i][1] < y_min: y_min = points[i][1]
    if points[i][1] > y_max: y_max = points[i][1]

# print(x_min, y_min, x_max, y_max)

trimmed = contour[y_min : y_max, x_min : x_max]
cv2.imwrite("trimmed_mask.jpg", trimmed)

# x_len, y_len = input("Please input your room size. (i.g. x_len, y_len)\n").split(",")
x_len = 28
y_len = 36

trimmed = cv2.resize(trimmed, dsize=(int(x_len),int(y_len)))
cv2.imwrite("trimmed_mask.jpg", trimmed)
room = [[0 for _ in range(int(x_len))] for _ in range(int(y_len))]

for i in range(int(y_len)):
    for j in range(int(x_len)):
        if trimmed[i][j][0] == 255: room[i][int(x_len)-1-j] = 1

with open('room.csv', 'w') as file:
    writer = csv.writer(file, lineterminator='\n')
    writer.writerows(room)