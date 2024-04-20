#ifndef DATA_NODE_H
#define DATA_NODE_H
#define Uses_TScrollBar
#define Uses_TLabel
#define Uses_TEventQueue
#define Uses_TCheckBoxes
#define Uses_THistory
#define Uses_TSItem
#define Uses_TForm
#define Uses_TInputLine
#define Uses_TSortedListBox
#define Uses_TDataCollection
#define Uses_TFileDialog
#define Uses_TStreamable
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TView
#define Uses_TButton
#define Uses_TOutline
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TApplication

#include <tvision/tv.h>
#include <string>

class DataNode : public TNode
{
	public:
		DataNode(std::string icon, std::string aText): 
			TNode(aText)
		{
			m_icon = icon;
			m_nodeName = aText;
		}
		DataNode(std::string icon, std::string aText, DataNode* aChildren, DataNode* aNext, Boolean initialState):
			  TNode(icon+aText, aChildren, aNext, initialState)
		{
			m_icon = icon;
			m_nodeName = aText;
		}
		std::string getName() { return m_nodeName;}
		std::string getIcon() { return m_icon;}
		void setName(std::string name) { m_nodeName = name;};
	private:
		std::string m_icon;
		std::string m_nodeName;
};

#endif
