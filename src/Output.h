
namespace stamping
{
class Output
{
public:
	static Output null;
	Output()
	: line_{0}, period_{0} {}
	Output(const int state, const int period)
	: line_{state},
	  period_{period}
	{}
	int line() const { return line_; }
	int period() const { return period_; }
private:
	int line_;
	int period_;
};
}
