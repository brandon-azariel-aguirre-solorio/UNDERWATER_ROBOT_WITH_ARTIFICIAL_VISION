import tensorflow as tf
from tensorflow.keras.preprocessing import image
import numpy as np

# Cargar el modelo guardado
# # Cargar el modelo
model = tf.keras.models.load_model(r'C:\Users\brand\source\repos\Python_objec\Python_objec\IA_submarino.h5')

# Cargar y preprocesar la imagen
import tensorflow as tf
from tensorflow.keras.preprocessing import image
import numpy as np

# Cargar el modelo guardado
# # Cargar el modelo
model = tf.keras.models.load_model(r'C:\Users\brand\source\repos\Python_objec\Python_objec\IA_submarino.h5')

# Cargar y preprocesar la imagen
img_path = r'C:\Users\brand\source\repos\Python_objec\FishImgDataset\train\Pez mediano\cephalopholis_spiloparaea_4.jpg'  # Cambia esto a la ruta correcta
target_size = (150, 150)  # Esto se basa en la arquitectura que has mostrado
class_names = ['Anguila', 'Ballena', 'Caballo de mar', 'Camaron', 
               'Cangrejo', 'Cocodrilo', 'Coral','Delphin', 'Erizo', 
               'Estrella de mar', 'Foca','Langosta', 
               'Mantarralla', 'Medusa', 'Nudibranchs','Nutria',
               'Orca','Ostra', 'Pez grande', 
               'Pez mediano', 'Pez chico', 'Pinguino', 'Pulpo', 
               'Tiburon', 'Tortuga']
# Cargar y preprocesar la imagen
img = image.load_img(img_path, target_size=target_size)
img_array = image.img_to_array(img)

# Asegúrate de que la forma es (250, 250 3)
#print(f'Forma antes de expandir: {img_array.shape}')  # Debería ser (250, 250, 3)

# Añadir dimensión para el batch
img_array = np.expand_dims(img_array, axis=0)  # Ahora debería ser (1, (250, 250, 3)
img_array /= 255.0  # Normalizar

# Verificar la forma de la entrada
#print(f'Forma de entrada: {img_array.shape}')  # Debe ser (1,(250, 250, 3)
import matplotlib.pyplot as plt
# Hacer la predicción
try:
    predictions = model.predict(img_array)
    predicted_class = np.argmax(predictions, axis=1)
    print(f'Clase predicha: {predicted_class}')
    # Mostrar la imagen
    plt.imshow(img_array[0])
    plt.title(f'Clase predicha: {predicted_class}')
    predicted_name = class_names[predicted_class[0]]
    plt.text(10, 10, f'Especie : {predicted_name}  ', fontsize=12, color='red', bbox=dict(facecolor='white', alpha=0.5))
    plt.axis('off')  # No mostrar ejes
    plt.show()
except Exception as e:
    print(f'Ocurrio un error durante la prediccion: {e}')
    import matplotlib.pyplot as plt
