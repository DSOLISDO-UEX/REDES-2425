# Organizacion del proyecto 

## Lenguaje usado
Hemos decidido hacer el proyecto en C++. Para ahorrarnos problemas al usar C y C++ hemos creado un puente entre la librería proporcionada (linkLayer) y nuestro proyecto. Este puente se compone de 3 librerias:
- Adapter, se encarga de todo lo relacionado con el adaptador en sí mismo.
- FrameBuilder, se encarga de construir los "frames" que luego se mandarán a traves del adaptador.
- Communication, se encarga de administrar todo lo relacionado con la estaciones esclava / maestra y su comunicacion.

## Estilo de codigo
Hemos decidido usar ClangFormat, basado en el estilo de Microsoft, para homogeneizar el código. Los métodos de las clases, así como sus atributos, están ordenados alfabéticamente según su tipo pero hemos tenido en cuenta algunas consideraciones:
- Los modificadores de tipo no afectan al orden (const, inline, static, ...)
- Los tipos de la librería estándar que estén dentro del espacio de nombres "std" ocuparán el último lugar.
  Si hubiera 2 o más tipos que empezaran por std::, estos se ordenaran alfabéticamente también.

En el caso de variables dentro de métodos, se sigue dentro de lo posible esta convención, aunque muchas veces por necesidades del código se puede romper esta estructura.  

## Modificaciones de .VSCODE
Se han modificado los archivos "tasks.json" y "launch.json" para que se añada el directorio "include" al path de compilación para acortar las rutas de los archivos incluidos. Tambien se ha fijado el main (Sesion4.cpp) para que no se tenga que estar en él a la hora de compilar el proyecto.

## Aspectos Generales
Se ha intentado seguir un diseño lo más modular posible y que permita agregar o quitar funcionalidades sin necesidad de modificar el núcleo del proyecto estando todo dividio y encapsulado en sus respectivas clases.

A la hora de agregar las funcionalidades que envían / reciben datos se ha usado un patrón de servicios. Se crea una clase que contenga todas las operaciones necesarias, y serán las estaciones quienes usen estas funcionalidades según les convenga. Se podría hacer una nueva clase "GenericService" que permita heredar de ella y definir ciertas operaciones comunes a todos los servicios pero no se dispuso de tiempo suficiente a la hora de realizar la última entrega.

Se hace un uso intensivo de std::string, lo cual no es lo más recomendado en términos de eficiencia y gestión de recursos pues cada vez que el objeto se mueve es necesario copiar / mover los datos que contiene. Se podría usar std::string_view pero forma parte del estándar de C++17. Siguiendo las recomendaciones del profesor de prácticas, se ha evitado el uso de características de cualquier estándar de C++ posterior al 14, por si hubiera algún problema en los ordenadores del laboratorio por no tenerlo instalado a la hora de realizar la defensa.

## Documentación
Todo el código esta documentado internamente siguiendo los estándares del propio lenguaje y sacando partido de lo que se conoce como "rich documentation", que permite visualizar la documentación formateada en el propio IDE al poner el ratón sobre un módulo, clase, estructura, ...

## Referencias
Aqui se listan las referencias usadas al hacer el proyecto:
- [Stack Overflow](https://stackoverflow.com/questions)
- [CppReference](https://en.cppreference.com/w/)
- [ChatGPT](https://chatgpt.com) (No usado para copiar código, solo para preguntar dudas acerca de como funcionan ciertos aspectos de C++ que no quedan claros en CppReference o Stack Overflow, como los [lambdas](https://en.cppreference.com/w/cpp/language/lambda))
- Guiones de prácticas.

# Autores
- Daniel Solís Domínguez (Computadores)
- María Belén Muñoz Ortiz (Software)