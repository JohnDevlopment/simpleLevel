#ifndef PDCOMMANDBUILDER_H_INCLUDED
#define PDCOMMANDBUILDER_H_INCLUDED

class PDCommand;

// template for a command builder
class PDCommandBuilder {
	public:
	
	PDCommandBuilder() {}
	virtual ~PDCommandBuilder() {}
	
	virtual PDCommand* Build() = 0;
};

// templated derived class of PDCommandBuilder; allocates a pointer to base class PDCommand
template<class CommandType>
class PDCommandTBuilder : public PDCommandBuilder {
	public:
	
	PDCommandTBuilder() {}
	virtual ~PDCommandTBuilder() {}
	
	virtual CommandType* Build();
};

template<class _CommandType>
_CommandType* PDCommandTBuilder<_CommandType>::Build() {
	return new _CommandType();
}

#endif /* PDCOMMANDBUILDER_H_INCLUDED */
