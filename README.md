# PROCESSAMENTO GRÁFICO 2025
Este repositório contém os projetos e as aplicações desenvolvidas em Processamento Gráfico.

## 📁 Estrutura do Projeto
- **assets/**: Contém todos os recursos como texturas, modelos 3D, fontes e outros arquivos de mídia usados no projeto.
- **include/**: Arquivos de cabeçalho para definição de classes, funções e interfaces.
- **src/**: Código-fonte principal em C++, onde está a implementação da lógica do programa.
- **shaders/**: Código GLSL para criação e manipulação de shaders gráficos.
- **third_party/**: Bibliotecas externas que o projeto utiliza, mantidas separadamente para facilitar a gestão.
- **tests/**: Testes automatizados para garantir a qualidade e funcionamento do código.
- **CMakeLists.txt**: Arquivo de configuração para o sistema de build CMake, que compila e organiza o projeto.
## 🔄 Usar o projeto
- Clonar o repositório no diretório escolhido.
- Abrir o projeto pela IDE (VS CODE):
   - abrir prompt de comando ou Git Bash dentro do diretório;
   - digitar o comando **code .** para abrir o projeto já no VS Code.
- No terminal, acessar pasta build e compilar:
   - cd build
   - cmake --build .
- Executar o binário gerado dentro da pasta build:
   - .\nomeProjeto
