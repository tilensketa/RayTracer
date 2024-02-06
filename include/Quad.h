class Quad {
  public:
    Quad();

    void draw();
    void clean();

  private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
};
