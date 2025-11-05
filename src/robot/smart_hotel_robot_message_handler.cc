#include "precomp.h"
#include "smart_hotel_robot_message_handler.h"

SmartHotelRobotMessageHandler::SmartHotelRobotMessageHandler(SmartHotelRobotContext* context)
    : _context(context)
{

}

SmartHotelRobotMessageHandler::~SmartHotelRobotMessageHandler()
{

}

bool SmartHotelRobotMessageHandler::OnQuery(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int64_t query_id,
    const CefString& request,
    bool persistent,
    CefRefPtr<Callback> callback)
{
    bool result = true;
    GumboOutput* output = nullptr;

    do
    {
        output = gumbo_parse_with_options(&kGumboDefaultOptions, request.ToString().c_str(), request.length());
        if (output == nullptr)
            break;

        SmartHotelRobot* robot = _context->GetHotelRobot();
        if (robot == nullptr)
            break;

        SearchForHotels(output->root, robot);

    } while (false);

    if (output)
    {
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        output = nullptr;
    }

    return result;
}

void SmartHotelRobotMessageHandler::SearchForHotels(GumboNode* node, SmartHotelRobot* robot)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    if (node->v.element.tag == GUMBO_TAG_DIV)
    {
        GumboAttribute* clazz = gumbo_get_attribute(&node->v.element.attributes, "class");
        GumboAttribute* index = gumbo_get_attribute(&node->v.element.attributes, "data-index");
        if (clazz && index && lstrcmpiA(clazz->value, "cell") == 0)
        {
            MessageRobotHotel hotel = { 0 };
            hotel.index = StrToIntA(index->value) + 1;
            GetHotel(node, hotel);
            robot->AddHotel(&hotel);
            return;
        }
    }

    GumboVector* children = &node->v.element.children;
    if (children)
    {
        for (unsigned int i = 0; i < children->length; ++i) {
            SearchForHotels(static_cast<GumboNode*>(children->data[i]), robot);
        }
    }
}

void SmartHotelRobotMessageHandler::GetHotel(GumboNode* node, MessageRobotHotel& result)
{
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* link = static_cast<GumboNode*>(children->data[i]);
        if (link == nullptr)
            continue;

        if (link->v.element.tag != GUMBO_TAG_A)
            continue;
        
        GumboVector* nodes = &link->v.element.children;
        for (unsigned int j = 0; j < nodes->length; j++)
        {
            GumboNode* child = static_cast<GumboNode*>(nodes->data[j]);
            if (child == nullptr)
                continue;

            if (child->v.element.tag != GUMBO_TAG_DIV)
                continue;

            GumboAttribute* child_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
            if (lstrcmpiA(child_attribute->value, "img-wrapper") == 0)
            {
                const char* image = GetHotelImage(child);
                if (image && lstrlenA(image))
                {
                    lstrcpyA(result.image, image);
                }
            }
            else if (lstrcmpiA(child_attribute->value, "wrapper") == 0)
            {
                GumboVector* rows = &child->v.element.children;
                for (unsigned int k = 0; k < rows->length; k++)
                {
                    GumboNode* row = static_cast<GumboNode*>(rows->data[k]);
                    if (row == nullptr)
                        continue;

                    if (row->v.element.tag != GUMBO_TAG_DIV)
                        continue;

                    GumboAttribute* row_attribute = gumbo_get_attribute(&row->v.element.attributes, "class");
                    if (row_attribute == nullptr)
                        continue;

                    if (lstrcmpiA(row_attribute->value, "row row1") == 0)
                    {
                        const char* title = GetHotelTitle(row);
                        if (title && lstrlenA(title))
                        {
                            lstrcpyA(result.title, title);
                        }
                    }
                    else if (lstrcmpiA(row_attribute->value, "row row2") == 0)
                    {
                        const char* score = GetHotelScore(row);
                        if (score && lstrlenA(score))
                        {
                            lstrcpyA(result.score, score);
                        }

                        const char* feedback = GetHotelFeedback(row);
                        if (feedback && lstrlenA(feedback))
                        {
                            lstrcpyA(result.feedback, feedback);
                        }

                        const char* star = GetHotelStar(row);
                        if (star && lstrlenA(star))
                        {
                            lstrcpyA(result.star, star);
                        }
                    }
                    else if (lstrcmpiA(row_attribute->value, "row row3") == 0)
                    {
                        const char* address = GetHotelAddress(row);
                        if (address && lstrlenA(address))
                        {
                            lstrcpyA(result.address, address);
                        }
                    }
                    else if (lstrcmpiA(row_attribute->value, "row row4") == 0)
                    {
                        const char* price = GetHotelPrice(row);
                        if (price && lstrlenA(price))
                        {
                            lstrcpyA(result.price, price);
                        }

                        const char* orig_price = GetHotelOrigPrice(row);
                        if (orig_price && lstrlenA(orig_price))
                        {
                            lstrcpyA(result.orig_price, orig_price);
                        }

                        const char* bought = GetHotelBought(row);
                        if (bought && lstrlenA(bought))
                        {
                            lstrcpyA(result.bought, bought);
                        }
                    }
                }
            }
        }
    }
}

const char* SmartHotelRobotMessageHandler::GetHotelImage(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++) 
    {
        GumboNode* image = static_cast<GumboNode*>(children->data[i]);
        if (image == nullptr)
            continue;

        if (image->v.element.tag != GUMBO_TAG_IMG)
            continue;

        GumboAttribute* imgage_src = gumbo_get_attribute(&image->v.element.attributes, "src");
        if (imgage_src)
        {
            return imgage_src->value;
        }
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelTitle(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_H1)
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if(text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelScore(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-score"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelFeedback(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-feedback"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelStar(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-star"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelAddress(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-address"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelPrice(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-price"))
            continue;

        GumboVector* prices = &child->v.element.children;
        if (prices == nullptr)
            continue;

        GumboNode* price = static_cast<GumboNode*>(prices->data[0]);
        if (price->v.element.tag != GUMBO_TAG_EM)
            continue;
        
        if (price->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(price->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelOrigPrice(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-origin"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}

const char* SmartHotelRobotMessageHandler::GetHotelBought(GumboNode* node)
{
    GumboVector* children = &node->v.element.children;
    if (children == nullptr)
        return nullptr;

    for (unsigned int i = 0; i < children->length; i++)
    {
        GumboNode* child = static_cast<GumboNode*>(children->data[i]);
        if (child == nullptr)
            continue;

        if (child->v.element.tag != GUMBO_TAG_SPAN)
            continue;

        GumboAttribute* class_attribute = gumbo_get_attribute(&child->v.element.attributes, "class");
        if (class_attribute == nullptr)
            continue;

        if (lstrcmpiA(class_attribute->value, "poi-bought"))
            continue;

        if (child->v.element.children.length == 0)
            continue;

        GumboNode* text = static_cast<GumboNode*>(child->v.element.children.data[0]);
        if (text->type != GUMBO_NODE_TEXT)
            continue;

        return text->v.text.text;
    }

    return nullptr;
}