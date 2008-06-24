class TextInfo():
  string = ""

  def __init__(self, string, cr=None):
    print("in init")
    self.string = string
    if cr is not None:
      print("all right! no crying in my bed tonight")
