#include "Neuron.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------------------------
void Neuron::IniticalNeuron(int inputCount, int hideCount, int outputCount)
{
	srand((unsigned)time(NULL));
	_inputNeuron = (InputNeuron**)malloc(sizeof(InputNeuron*) * (inputCount + 1));
	_hideNeuron = (HideNeuron**)malloc(sizeof(HideNeuron*) * (hideCount + 1));
	_outputNeuron = (OutputNeuron**)malloc(sizeof(OutputNeuron*) * outputCount);

	_inputCount = inputCount + 1;
	_hideCount = hideCount;
	_outputCount = outputCount;

	_lineCount = (_inputCount) * _hideCount + (_hideCount+1) * _outputCount;
	_linkLine = (LinkLine*)malloc(sizeof(LinkLine) *_lineCount);

	for (int i = 0; i < _outputCount; i++)
	{
		(*(_outputNeuron + i)) = (OutputNeuron*)malloc(sizeof(OutputNeuron));
	}

	for (int i = 0; i < _lineCount; i++)
	{				
		(_linkLine + i)->weight = 1 - 2 * rand() / double(RAND_MAX); 
		(_linkLine + i)->total = 0;
		(_linkLine + i)->count = 0;
		if (i < _inputCount * _hideCount)
		{
			int offset = i / _hideCount;
			int pos = i - offset * _hideCount;
			if (pos == 0)
			{
				(*(_inputNeuron + offset)) = (InputNeuron*)malloc(sizeof(InputNeuron));
				(*(_inputNeuron + offset))->InitLine(_hideCount);
			}
			(*(_inputNeuron + offset))->addNewLine((_linkLine + i));
		}
		else
		{
			int count = i - _inputCount * _hideCount;
			int offset = count / _outputCount;
			int pos = count - offset * _outputCount;
			if (pos == 0)
			{
				(*(_hideNeuron + offset)) = (HideNeuron*)malloc(sizeof(HideNeuron));
				(*(_hideNeuron + offset))->InitLine(_outputCount);
			}
			(*(_hideNeuron + offset))->addNewLine((_linkLine + i));
		}
	}
	(*(_inputNeuron + 0))->value = 1;

	(*(_hideNeuron + hideCount))->value = 1;

	for (int i = 0; i < _lineCount; i++)
	{
		printf("link:%d,weight:%f\n", i, (_linkLine + i)->weight);
	}
}

//-------------------------------------------------------------------------------------------
double Neuron::WeightedSumInput(int pos)
{
	double total = 0;
	for (int i = 0; i < _inputCount; i++)
	{
		InputNeuron* temp = *(_inputNeuron + i);
		LinkLine* tempLine = *(temp->linkline + pos);

		total += temp->value * tempLine->weight;
	}
//	printf("SumInput:%f\n", total);
	return total;
}
//-------------------------------------------------------------------------------------------
double Neuron::WeightedSumHide(int pos)
{
	double total = 0;

	for (int i = 0; i < _hideCount; i++)
	{
		HideNeuron* temp = *(_hideNeuron + i);
		LinkLine* tempLine = *(temp->linkline + pos);
		temp->value = (this->*_func)(WeightedSumInput(i));
//		temp->value = calcPurelinFire(WeightedSumInput(i));
		total += temp->value * tempLine->weight;
	}
	HideNeuron* temp = *(_hideNeuron + _hideCount);
	LinkLine* tempLine = *(temp->linkline + pos);
	total += temp->value * tempLine->weight;
	return total;
}

double Neuron::Output()
{
	double result1 = WeightedSumHide(0);
	(*_outputNeuron)->value = (this->*_func)(result1);
	double resultdesired = (this->*_func)((*_outputNeuron)->desiredvalues);

	double result = (*_outputNeuron)->value;
	double tempvalue = result - resultdesired;
//	double tempvalue = result - (*_outputNeuron)->desiredvalues;
	tempvalue = tempvalue * tempvalue;
	if (tempvalue > desiredPersent)
	{
//		double residual = -(result - (*_outputNeuron)->desiredvalues)*result*(1 - result);
		double residual = -(result - resultdesired)*result*(1 - result);
		for (int i = 0; i < _hideCount; i++)
		{
			HideNeuron* hidetemp = *(_hideNeuron + i);
			LinkLine* linetemp = *hidetemp->linkline;
			double weightsum = linetemp->weight * residual;
			double hideresidual = weightsum * hidetemp->value * (1 - hidetemp->value);
			for (int j = 0; j < _inputCount; j++)
			{
				InputNeuron* inputtemp = *(_inputNeuron + j);
				LinkLine* inputlinetemp = *(inputtemp->linkline+i);
				inputlinetemp->addWeight(inputtemp->value*hideresidual*study);
			}
			linetemp->addWeight(hidetemp->value*residual*study);
		}
	}
	return tempvalue;
}

double Neuron::calcLogsigFire(double temp)
{
	double value = 1.0 / (1.0 + exp(-temp));
//	double value = (exp(temp) - exp(-temp)) / (exp(temp) + exp(-temp));
	
	return value;
}

double Neuron::calcsinFire(double temp)
{
	double value = sin(temp);
	//	double value = (exp(temp) - exp(-temp)) / (exp(temp) + exp(-temp));

	return value;
}

double Neuron::calcPurelinFire(double value)
{
	return value;
}

void Neuron::GetAllWeight()
{
	for (int i = 0; i < _lineCount; i++)
	{
		printf("link:%d,weight:%f\n", i, (_linkLine + i)->weight);
	}
}

void Neuron::SetWeight(double x1, double x2, double x3, double x4)
{
	(_linkLine + 0)->weight = x1;
	(_linkLine + 1)->weight = x2;
	(_linkLine + 2)->weight = x3;
	(_linkLine + 3)->weight = x4;
}

//-------------------------------------------------------------------------------------------
void Neuron::setValue2(double x1, double x2, double expected, char* funcname)
{
	_func = &Neuron::calcLogsigFire;
	if (strcmp(funcname,"logsig") == 0)
		_func = &Neuron::calcLogsigFire;
	if (strcmp(funcname, "sin") == 0)
		_func = &Neuron::calcsinFire;
	if (strcmp(funcname, "purelin") == 0)
		_func = &Neuron::calcPurelinFire;

	InputNeuron* temp = *(_inputNeuron + 1);
	temp->value = (this->*_func)(x1);
	temp = *(_inputNeuron + 2);
	temp->value = (this->*_func)(x2);

	OutputNeuron* outtemp = *_outputNeuron;
	outtemp->setDesiredValues(expected);
}

//-------------------------------------------------------------------------------------------
double Neuron::GetAnswr(double x1, double x2, double x3)
{
	InputNeuron* temp = *(_inputNeuron + 1);
	temp->value = x1;
	temp = *(_inputNeuron + 2);
	temp->value = x2;
	temp = *(_inputNeuron + 3);
	temp->value = x3;

	double total = 0;

	for (int i = 0; i < _hideCount; i++)
	{
		HideNeuron* temp = *(_hideNeuron + i);
		LinkLine* tempLine = *(temp->linkline + 0);
		temp->value = (WeightedSumInput(i));
		total += temp->value * tempLine->weight;
	}
	return total;
}

//-------------------------------------------------------------------------------------------
double Neuron::GetAnswr(double x1, double x2)
{
	InputNeuron* temp = *(_inputNeuron + 1);
	temp->value = x1;
	temp = *(_inputNeuron + 2);
	temp->value = x2;

	double total = 0;

	for (int i = 0; i < _hideCount; i++)
	{
		HideNeuron* temp = *(_hideNeuron + i);
		LinkLine* tempLine = *(temp->linkline + 0);
		temp->value = (WeightedSumInput(i));
		total += temp->value * tempLine->weight;
	}
	return total;
}

//-------------------------------------------------------------------------------------------
void Neuron::FreeNeuron()
{
	for (int i = 0; i < _inputCount; i++)
	{
		(*(_inputNeuron + i))->FreeLine();
	}
	for (int i = 0; i <= _hideCount; i++)
	{
		(*(_hideNeuron + i))->FreeLine();
	}
	free(_inputNeuron);
	free(_hideNeuron);
	free(_outputNeuron);
}