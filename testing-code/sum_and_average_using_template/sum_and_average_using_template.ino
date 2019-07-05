template <class ta, class tb>
class test {
    ta s;
  public:
    void input(ta a[], tb b);
    
    float sum();
    
};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 float arrayd[10]={2,300,4,5,6,7,8,9,10,11};
  test<float, int> t1;
  t1.input(arrayd, 10);
  Serial.println(t1.sum());
}

void loop() {
  // put your main code here, to run repeatedly:

}

template <class ta, class tb>
float test<ta, tb>::sum() {
  return s;
}

template <class ta, class tb>
void test <ta, tb>::input(ta a[], tb size) {
  this->s=0;
  for (int i = 0; i < size; i++) {
    this->s = a[i]+this->s;
  }
  return 1;
}
template <class ta, class tb>
void test <ta, tb>::input(ta a[], tb size) {
  this->s=0;
  for (int i = 0; i < size; i++) {
    this->s = a[i]+this->s;
  }
  return 1;
}
