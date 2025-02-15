# Organizacion del proyecto 
Hemos decidido hacer el proyecto en C++. Para ahorrarnos problemas al usar C y C++ hemos creado un puente entre la librería proporcionada (linkLayer) y nuestro proyecto. Este puente se compone de 2 librerias:
- Adapter, se encarga de todo lo relacionado con el adaptador en si mismo.
- FrameBuilder, se encarga de construir los "frames" que luego se mandarán a traves del adaptador.

# Estilo de codigo
Hemos decidido usar ClangFormat, basado en el estilo de Microsoft, para homogeneizar el código. Los métodos de las clases, asi como sus atributos, están ordenados alfabeticamente segun su tipo pero hemos tenido en cuenta algunas consideraciones:
- Los modificadores de tipo no afectan al orden (const, inline, static, ...)
- Los tipos de la libreria estandar que esten dentro del espacio de nombres "std" ocuparan el último lugar.
  Si hubiera 2 o más tipos que empezaran por std::, estos se ordenaran alfabeticamente tambien.

En el caso de variables dentro de métodos, se sigue dentro de lo posible esta convención, aunque muchas veces por necesidades del código se puede romper esta estructura.  

# Autores
- Daniel Solís Domínguez (Computadores)
- María Belén Muñoz Ortiz (Software)