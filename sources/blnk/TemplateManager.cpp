#include <TemplateManager.h>
#include <lms_entrytemplate_primitive_orm.h>


void BlnkTemplateManager::buildLegs()
{

}
void BlnkTemplateManager::loadTemplate (int template_id)
{
    

}
bool BlnkTemplateManager::validate ()
{

}
BlnkTemplateManager::BlnkTemplateManager(int template_id)
{
    this->getTemplate(template_id);

}
bool BlnkTemplateManager::buildEntry (json temp_amount_json)
{

}
void BlnkTemplateManager::getTemplate (int template_id)
{
    lms_entrytemplate_primitive_orm_iterator * _lms_entrytemplate_primitive_orm_iterator = new lms_entrytemplate_primitive_orm_iterator("main");

    _lms_entrytemplate_primitive_orm_iterator->filter(
        UnaryOperator ("lms_entrytemplate.id",eq,template_id)
    );

    _lms_entrytemplate_primitive_orm_iterator->execute();

    lms_entrytemplate_primitive_orm * temp = _lms_entrytemplate_primitive_orm_iterator->next();
    json _template = temp->get_template();
    this->template_legs = _template;
    cout << "TEMPLATE" << _template.dump();

    delete(_lms_entrytemplate_primitive_orm_iterator);
}

BlnkTemplateManager::~BlnkTemplateManager()
{

}
