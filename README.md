# PROCESSAMENTO GRÁFICO 2025
Este repositório contém os projetos e as aplicações desenvolvidas em Processamento Gráfico.

## 📁 Project structure

- assets: Contém todos os recursos como texturas, modelos 3D, fontes e outros arquivos de mídia usados no projeto.
- include: Arquivos de cabeçalho para definição de classes, funções e interfaces.
- src: Código-fonte principal em C++, onde está a implementação da lógica do programa.
- shaders: Código GLSL para criação e manipulação de shaders gráficos.
- third_party: Bibliotecas externas que o projeto utiliza, mantidas separadamente para facilitar a gestão.
- tests: Testes automatizados para garantir a qualidade e funcionamento do código.
- CMakeLists.txt: Arquivo de configuração para o sistema de build CMake, que compila e organiza o projeto.

## 🔄 Steps to reproduce
- Clone repository
- Ctrl+Shift+P > Select a Kit > GCC
- cd build
- cmake --build .
- Executar o binário gerado dentro da pasta build: 
   - 'cmd /c {fileName}.exe' OU
   - '.\ {filename}'

 ## Files
| FileName       | Description                     | Colleagues                                                      |
|----------------|---------------------------------|-----------------------------------------------------------------|
| `Vivencial01`  | Triângulos de tamanhos variados | Bruno Gerling e Mariana Sales                                   |
| `DesafioM2`    | Triângulos Personalizados       | Bruno Gerling                                                   |
| `DesafioM4`    | Hitboxes em sprites             | Bruno Gerling                                                   |
| `DesafioM5`    | Animação                        | Bruno Gerling                                                   |


