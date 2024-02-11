from ultralytics import YOLO
import os

os.environ['KMP_DUPLICATE_LIB_OK']='True'

# Load the model.
# Load a pretrained YOLOv8n model
model = YOLO('runs/detect/train2/weights/best.pt')

# Define current screenshot as source
# Run inference on the source
results = model.predict(
   imgsz=640,
   source = '0',
   show = True,
)
