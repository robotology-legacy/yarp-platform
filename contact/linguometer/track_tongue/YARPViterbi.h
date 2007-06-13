#ifndef YARPVITERBI_INC
#define YARPVITERBI_INC

#include <assert.h>
#include <yarp/sig/Image.h>

class YARPViterbi
{
public:
  int K;
  int T;
  int index;
  int mode;
  int path_valid;
  float best_cost;
  yarp::sig::ImageOf<float> cost;
  yarp::sig::ImageOf<int> src;
  yarp::sig::ImageOf<int> path;

  YARPViterbi()
    {
      K = T = 0;
      Reset();
    }

  void SetSize(int states, int sequence_length);

  void Reset()
    {
      index = 0;
      mode = 0;
      path_valid = 0;
      best_cost = 0;
    }

  void AssertMode(int n_mode);

  void AddTransition(int s0, int s1, float c);

  void EndTransitions()
    {
      path_valid = 0;
      AssertMode(0);
    }

  void BeginTransitions()
    {
      path_valid = 0;
      AssertMode(1);
    }

  void CalculatePath();

  void ShowPath();

  int length()
    {
      if (index>0)
	{
	  CalculatePath();
	}
      return index;
    }

  int GetPath(int i)
    {
      CalculatePath();
      assert(i<index);
      return path(0,i);
    }

  int operator() (int i)
    {
      return GetPath(i);
    }

  float GetCost()
    {
      CalculatePath();
      return best_cost;
    }

  /*
  void CopyPath(YARPGenericImage& out_path)
  {
	  CalculatePath();
	  out_path.CastCopy(path);
  }
  */
};


#endif
