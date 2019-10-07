
#ifndef PDCOMPONENTBUILDER_H_INCLUDED
#define PDCOMPONENTBUILDER_H_INCLUDED

// forward declaration
class PDComponent;

/* Interface of a component builder */
class PDComponentBuilder {
	public:
	
	PDComponentBuilder() = default;
	virtual ~PDComponentBuilder() {}
	
	// return a pointer to a newly allocated component
	virtual PDComponent* Build() = 0;
};

/* Templated derived class of PDComponentBuilder */
template<class CompType>
class PDComponentTBuilder : public PDComponentBuilder {
	public:
	
	virtual PDComponent* Build();
};

template<class CompType>
PDComponent* PDComponentTBuilder<CompType>::Build()
{
	return new CompType();
}

#endif /* PDCOMPONENTBUILDER_H_INCLUDED */
