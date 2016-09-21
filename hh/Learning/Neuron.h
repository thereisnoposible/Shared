#pragma once
#include <stdlib.h>

struct LinkLine
{
	void addWeight(double _weight)
	{
		total += _weight;
		count += 1;
		if (count >= 1000)
		{
			weight += total / count;
			total = 0;
			count = 0;
		}
	}
	double weight;
	double total;
	double count;
};

struct InputNeuron
{
	void InitLine(int count)
	{
		linkline = (LinkLine**)malloc(sizeof(LinkLine*)* count);
		lineCount = 0;
	}
	void FreeLine()
	{
		free(linkline);
	}
	void addNewLine(LinkLine* temp)
	{
		*(linkline + lineCount) = temp;
		lineCount++;
	}
	double value;
	int lineCount;
	LinkLine** linkline;
};

struct HideNeuron
{
	void InitLine(int count)
	{
		linkline = (LinkLine**)malloc(sizeof(LinkLine*)* count);
		lineCount = 0;
	}
	void FreeLine()
	{
		free(linkline);
	}
	void addNewLine(LinkLine* temp)
	{
		*(linkline + lineCount) = temp;
		lineCount++;
	}
	double value;
	int lineCount;
	LinkLine** linkline;
};

struct OutputNeuron
{
	void setDesiredValues(double temp)
	{
		desiredvalues = temp;
	}
	double value;
	double desiredvalues;
};

struct Neuron
{
	void IniticalNeuron(int inputCount, int hideCount, int outputCount);
	void FreeNeuron();

	double WeightedSumInput(int pos);
	double WeightedSumHide(int outputpos);
	double Output();

	double calcLogsigFire(double value);
	double calcPurelinFire(double value);
	double calcsinFire(double temp);

	void GetAllWeight();

	void SetWeight(double x1, double x2, double x3, double x4);

	void setValue2(double x1, double x2, double expected,char* funcname);

	double GetAnswr(double x1, double x2, double x3);
	double GetAnswr(double x1, double x2);
private:
	InputNeuron** _inputNeuron;
	LinkLine* _linkLine;
	HideNeuron** _hideNeuron;
	OutputNeuron** _outputNeuron;

	double (Neuron::*_func)(double value);


	int _inputCount;
	int _hideCount;
	int _outputCount;
	int _lineCount;

	const double study = 1;
	const double desiredPersent = 0.0000001;
};
