from ultralytics import YOLO
import os

train_images_dir = r'C:\Users\brand\vidamarinayolo\vidamarina_2.0_train\images'  
train_labels_dir = r'C:\Users\brand\vidamarinayolo\vidamarina_2.0_train\labels'
val_images_dir = r'C:\Users\brand\vidamarinayolo\vidamarina_2.0\images'     
val_labels_dir = r'C:\Users\brand\vidamarinayolo\vidamarina_2.0\labels'       

num_classes = 17  # Reemplaza con el número real de tus clases
class_names =['Anguila','Ballena','Cangrejo','Cocodrilo','Delphin','Erizo','Estrella','Mantarraya','Medusa','Orca','Pez_Chico','Pez_Grande','Pez_Mediano','Pulpo','Sea_horse','Tiburon','Tortuga']  # Reemplaza con los nombres de tus clases

data_yaml_content = f"""
train: {train_images_dir}
val: {val_images_dir if val_images_dir else ''}

nc: {num_classes}
names: {class_names}
"""
with open('temp_dataset.yaml', 'w') as f:
    f.write(data_yaml_content)

yaml_file = 'temp_dataset.yaml'

ruta_a_tu_modelo_entrenado = r'C:\Users\brand\runs\detect\mi_entrenamiento_yolov11l\mi_entrenamiento_yolov11m\weights\best.pt'  # Reemplaza con la ruta real de tu archivo .pt
model = YOLO(ruta_a_tu_modelo_entrenado)

results = model.train(data=yaml_file, epochs=2, batch=32, imgsz=640, name='mi_reentrenamiento')

if val_images_dir and val_labels_dir:
    metrics = model.val(data=yaml_file)
    print(metrics)
else:
    print("No se proporcionaron directorios de validación, la evaluación se omitió.")


os.remove('temp_dataset.yaml')

print("¡Reentrenamiento completado!")
