object player
{
    required uuid: string!;
    required name: string!;
    
    method __compare__() = uuid;
};