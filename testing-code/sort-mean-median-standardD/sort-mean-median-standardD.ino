template <class ta>
class test {
    float s;
    ta md;
    float mn;

  public:
    int partition(ta *a, int start, int end);
    void Quicksort(ta *a, int start, int end);
    ta sum(ta *a, int size);
    float calculateSD(ta *a, int size);
    ta median(ta *a, int size);
    float mean(ta *a, int size);
};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int arrayd[] = {3, 4, 4, 3, 4, 3, 4};
  int size = sizeof(arrayd) / sizeof(int);
  test<int> t1;
  t1.Quicksort(arrayd, 0, size - 1);
  for (int i = 0; i < size; i++)
  {
    Serial.println(arrayd[i]);
  }
  Serial.println(t1.sum(arrayd, size));
  Serial.println(t1.mean(arrayd, size));
  Serial.println(t1.median(arrayd, size));
  Serial.println(t1.calculateSD(arrayd, size));
}

void loop() {
  // put your main code here, to run repeatedly:

}

template <class ta>
void test<ta>::Quicksort(ta *a, int start, int end) {
  if (start < end)
  {
    int P_index = partition(a, start, end);
    Quicksort(a, start, P_index - 1);
    Quicksort(a, P_index + 1, end);
  }
}

template <class ta>
int test <ta>::partition(ta *a, int start, int end) {
  int pivot = a[end];
  //P-index indicates the pivot value index

  int P_index = start;
  ta t; //t is temporary variable

  for (int i = start; i < end; i++)
  {
    if (a[i] <= pivot)
    {
      t = a[i];
      a[i] = a[P_index];
      a[P_index] = t;
      P_index++;
    }
  }
  //Now exchanging value of
  //pivot and P-index
  t = a[end];
  a[end] = a[P_index];
  a[P_index] = t;

  //at last returning the pivot value index
  return P_index;
}
template <class ta>
ta test <ta>::sum(ta *a, int size) {
  this->s = 0;
  for (int i = 0; i < size; i++) {
    this->s = a[i] + this->s;
  }
  return this->s;
}
template <class ta>
ta test <ta>::median(ta *a, int size) {
  this->md = 0;
  if (size % 2 == 0) {
    this->md = a[size / 2];
  }
  else {
    int no = size / 2;
    this->md = a[++no];
  }
  return this->md;
}
template <class ta>
float test <ta>::mean(ta *a, int size) {
  this->mn = this->s / size;
  Serial.println("mean");
  Serial.println(this->s);
  Serial.println(size);
  return this->mn;
}
template <class ta>
float test <ta>::calculateSD(ta *a, int size)
{
  float  standardDeviation = 0.0;
  for (int i = 0; i < size; ++i)
    standardDeviation += pow(a[i] - this->mn, 2);
  return sqrt(standardDeviation / size);
}
