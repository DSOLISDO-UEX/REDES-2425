# Organizacion del proyecto 
Hemos decidido hacer el proyecto en C++. Para ahorrarnos problemas al usar C y C++ hemos creado un puente entre la librería proporcionada (linkLayer) y nuestro proyecto. Este puente se compone de 2 librerias:
- Adapter, se encarga de todo lo relacionado con el adaptador en sí mismo.
- FrameBuilder, se encarga de construir los "frames" que luego se mandarán a traves del adaptador.
- Communication, se encarga de administrar todo lo relacionado con la estaciones esclava / maestra.

# Estilo de codigo
Hemos decidido usar ClangFormat, basado en el estilo de Microsoft, para homogeneizar el código. Los métodos de las clases, así como sus atributos, están ordenados alfabéticamente según su tipo pero hemos tenido en cuenta algunas consideraciones:
- Los modificadores de tipo no afectan al orden (const, inline, static, ...)
- Los tipos de la librería estándar que estén dentro del espacio de nombres "std" ocuparán el último lugar.
  Si hubiera 2 o más tipos que empezaran por std::, estos se ordenaran alfabéticamente tambien.

En el caso de variables dentro de métodos, se sigue dentro de lo posible esta convención, aunque muchas veces por necesidades del código se puede romper esta estructura.  

# Autores
- Daniel Solís Domínguez (Computadores)
- María Belén Muñoz Ortiz (Software)